#include "Net.h"

namespace tensorflow
{

void Net::inference(const Tensor& input, std::vector<Tensor> *outputs)
{
    TF_CHECK_OK(session->Run({ {"x:0", input} }, {"prob:0", "value:0"}, {}, outputs));
}

void Net::train(const Tensor& input, const Tensor& labels, const Tensor& values, std::vector<Tensor> *outputs)
{
    TF_CHECK_OK(session->Run(
        { {"x:0", input}, {"labels:0", labels}, {"y:0", values} },
        { "loss:0", "global_step:0" },
        { "train_op" },
        outputs
    ));
}

void Net::initGraph()
{
    TF_CHECK_OK(session->Run({}, {}, {"init"}, nullptr));
}

Status Net::save(const string& ckpt)
{
    Tensor t(DT_STRING, TensorShape());
    t.scalar<tstring>()() = ckpt;
    return session->Run({{"save/Const:0", t}}, {}, {"save/control_dependency:0"}, nullptr);
}

Status Net::restore(const string& ckpt)
{
    Tensor t(DT_STRING, TensorShape());
    t.scalar<tstring>()() = ckpt;
    return session->Run({{"save/Const:0", t}}, {}, {"save/restore_all"}, nullptr);
}

}
