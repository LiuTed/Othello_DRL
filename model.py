import tensorflow.compat.v1 as tf
from tensorflow.compat.v1 import layers

inputs = tf.placeholder(tf.float32, shape=(None, 8, 8, 8), name = 'x')
labels = tf.placeholder(tf.float32, shape=(None, 65), name = 'labels')
y = tf.placeholder(tf.float32, shape=(None, 1), name = 'y')

def ResBlock(input, channel, kernel, name):
    with tf.name_scope(name):
        conv1 = layers.Conv2D(channel, kernel, padding = 'same')(input)
        conv1 = layers.BatchNormalization()(conv1)
        conv1 = tf.nn.relu(conv1, name = 'conv1')
        conv2 = layers.Conv2D(channel, kernel, padding = 'same')(conv1)
        conv2 = layers.BatchNormalization()(conv2)
        conv2 = tf.nn.relu(conv2, name = 'conv2')
        add_channel = tf.nn.relu(layers.Conv2D(channel, 1)(input))
        return tf.add(conv2, add_channel)

channels = [32, 64, 128, 256, 512]
res = inputs
for i, c in enumerate(channels):
    res = ResBlock(res, c, 3, 'conv%d'%(i+1))
    if i == 2:
        res = layers.MaxPooling2D(2, 2)(res) # board has only 8*8, thus only need to pool one time

pool = tf.math.reduce_max(res, [1, 2])
#dense layer, will be followed by two independent dense layers
dense = layers.Dense(512, activation = tf.nn.relu, kernel_regularizer = tf.nn.l2_loss, name = 'dense')(pool)

logits = layers.Dense(65, kernel_regularizer = tf.nn.l2_loss, name = 'logits')(dense)
prob = tf.nn.softmax(logits, name = 'prob')
value = tf.math.tanh(layers.Dense(1, kernel_regularizer = tf.nn.l2_loss)(dense), name = 'value')

label_loss = tf.nn.softmax_cross_entropy_with_logits_v2(labels, logits, name = 'label_loss')
value_loss = tf.math.square(tf.subtract(value, y), name = 'value_loss')
regularization_loss = tf.reduce_sum(tf.get_collection(tf.GraphKeys.REGULARIZATION_LOSSES), name = 'regularization_loss')

loss = tf.add(tf.math.reduce_mean(tf.add(label_loss, value_loss)), regularization_loss, name = 'loss')
global_step = tf.Variable(0, name = 'global_step')
opt = tf.train.AdamOptimizer(3e-4, epsilon = 1e-7)
train_op = opt.minimize(loss, global_step = global_step, name = 'train_op')

init = tf.global_variables_initializer()

saver_def = tf.train.Saver().as_saver_def()

for i in [inputs, labels, y, prob, value, loss, global_step, train_op, init]:
    print(i.name)

print('filename: ' + saver_def.filename_tensor_name)
print('save tensor: ' + saver_def.save_tensor_name)
print('restore tensor: ' + saver_def.restore_op_name)

with open('graph.pb', 'wb') as f:
    f.write(tf.get_default_graph().as_graph_def().SerializeToString())
