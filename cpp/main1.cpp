#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <filesystem>
using namespace std;

// 定义流的结构体
struct Flow {
    int id;             // 流的id
    int bandwidth;      // 流的带宽
    int enter_time;     // 流的进入设备时间
    int send_time;      // 流的发送时间
    int start_time;     // 流的开始发送时间
};

// 定义端口的结构体
struct Port {
    int id;             // 端口id
    int bandwidth;      // 端口带宽
    int free_bandwidth; // 端口当前空闲带宽
};

// 用于比较端口空闲带宽大小的函数
bool cmp_port_free_bandwidth(const Port& a, const Port& b) {
    return a.free_bandwidth > b.free_bandwidth;
}

// 用于比较流带宽大小的函数
bool cmp_flow_bandwidth(const Flow& a, const Flow& b) {
    return a.bandwidth > b.bandwidth;
}

int main(){
    //遍历文件夹
    for(int i = 0 ;;i++){
        // 读取端口文件
        ifstream port_file("../data/" + std::to_string(i) + "/port.txt");
        if (!port_file) {
            cerr << "Error: Failed to open port file in " << endl;
            break;
        }
        string port_line;
        getline(port_file, port_line);  // 第一行是列信息，不用读取
        vector<Port> ports;
        while (getline(port_file, port_line)) {
            Port port;
            stringstream ss(port_line);
            string token;
            getline(ss, token, ',');
            port.id = stoi(token);
            getline(ss, token, ',');
            port.bandwidth = stoi(token);
            port.free_bandwidth = port.bandwidth;
            ports.push_back(port);
        }
        port_file.close();

        //读取流文件
        ifstream flow_file("../data/" + std::to_string(i) + "/flow.txt");
        string flow_line;
        getline(flow_file, flow_line);  // 第一行是列信息，不用读取
        vector<Flow> flows;
        while (getline(flow_file, flow_line)) {
            Flow flow;
            stringstream ss(flow_line);
            string token;
            getline(ss, token, ',');
            flow.id = stoi(token);
                flow.bandwidth = stoi(token);
            getline(ss, token, ',');
            flow.enter_time = stoi(token);
            getline(ss, token, ',');
            flow.send_time = stoi(token);
            flows.push_back(flow);
        }
        flow_file.close();

        // 按照流的带宽从大到小排序
        sort(flows.begin(), flows.end(), cmp_flow_bandwidth);

        // 用优先队列存储每个端口的等待队列
        priority_queue<Flow, vector<Flow>, bool(const Flow&, const Flow&)> waiting_queues(
                [](const Flow& a, const Flow& b) {
                return a.start_time > b.start_time;
                });

        // 遍历每个流，计算其发送时间和端口使用情况
        for (auto& flow : flows) {
        // 更新时间
            int time = max(time, flow.enter_time);

        // 从空闲带宽最大的端口开始遍历，直到找到一个空闲带宽大于等于该流带宽的端口
        auto it = lower_bound(ports.begin(), ports.end(), Port{-1, flow.bandwidth, -1}, cmp_port_free_bandwidth);
        if (it == ports.end() || it->free_bandwidth < flow.bandwidth) {
            // 无可用端口，将流加入等待队列
            waiting_queues.push(flow);
        } else {
            // 有可用端口，更新端口的空闲带宽和流的发送时间
            it->free_bandwidth -= flow.bandwidth;
                // 初始化时间为第一个流进入设备的时间
        int time = flows[0].enter_time;

        // 用优先队列存储每个端口的等待队列
        priority_queue<Flow, vector<Flow>, bool(const Flow&, const Flow&)> waiting_queues(
            [](const Flow& a, const Flow& b) { return a.start_time > b.start_time; });

        // 逐个流处理
        for (auto& flow : flows) {
            // 找到空闲带宽最大的端口
            auto it = max_element(ports.begin(), ports.end(), cmp_port_free_bandwidth);
            if (it == ports.end()) {
                cerr << "Error: No available port" << endl;
                return 1;
            }
            // 如果该端口空闲带宽小于流的带宽，将流插入该端口的等待队列中
            if (it->free_bandwidth < flow.bandwidth) {
                waiting_queues.push(flow);
                continue;
            }
            // 否则，该端口空闲带宽足够，可以将流发送出去
            it->free_bandwidth -= flow.bandwidth;
            flow.start_time = time;
            // 更新时间为流开始发送的时间
            time = max(time, flow.send_time);
            // 统计发送成功的流的数量和总带宽
            static int success_flows = 0, total_bandwidth = 0;
            success_flows++;
            total_bandwidth += flow.bandwidth;
            // 将该端口之前等待队列中的流按照发送时间排序，依次尝试发送
            while (!waiting_queues.empty() && it->free_bandwidth >= waiting_queues.top().bandwidth) {
                auto waiting_flow = waiting_queues.top();
                waiting_queues.pop();
                it->free_bandwidth -= waiting_flow.bandwidth;
                waiting_flow.start_time = time;
                // 更新时间为流开始发送的时间
                time = max(time, waiting_flow.send_time);
                success_flows++;
                total_bandwidth += waiting_flow.bandwidth;
            }
        }

        // 输出结果
        ofstream result_file("result.txt");
        if (!result_file) {
            cerr << "Error: Failed to open result file" << endl;
            return 1;
        }

        // 输出结果到文件
        for (const auto& flow : flows) {
            result_file << flow.id << "," << flow.send_time << "," << flow.start_time << endl;
        }

        // 关闭输出文件
        result_file.close();
        }

    }
    return 0;
}
