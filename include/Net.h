#ifndef __NET_H__
#define __NET_H__

#include "tensorflow/cc/ops/standard_ops.h"
#include "tensorflow/core/framework/graph.pb.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/graph/default_device.h"
#include "tensorflow/core/graph/graph_def_builder.h"
#include "tensorflow/core/lib/core/threadpool.h"
#include "tensorflow/core/lib/strings/str_util.h"
#include "tensorflow/core/lib/strings/stringprintf.h"
#include "tensorflow/core/platform/init_main.h"
#include "tensorflow/core/platform/logging.h"
#include "tensorflow/core/platform/types.h"
#include "tensorflow/core/public/session.h"

namespace tensorflow
{

class Net
{
    std::unique_ptr<Session> session;
    string path;
    GraphDef graph;
public:
    Net(const string& graphPath, const SessionOptions &opt = SessionOptions()):
        session(NewSession(opt)), path(graphPath), graph()
    {
        TF_CHECK_OK(ReadBinaryProto(Env::Default(), path, &graph));
        TF_CHECK_OK(session->Create(graph));
    }
    //outputs [policy, value]
    void inference(const Tensor& input, std::vector<Tensor> *outputs);
    void train(const Tensor& input, const Tensor& labels, const Tensor& values, std::vector<Tensor> *outputs);
    void initGraph();
    Status save(const string& ckpt);
    Status restore(const string& ckpt);
};

}

#endif
