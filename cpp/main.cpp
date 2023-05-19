#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <algorithm>

using namespace std;

class Flow {
public:
    int id, bandwidth, enter_time, send_time, start_time, port_id;
    Flow(int f_id, int bw, int e_time, int s_time) : 
        id(f_id), bandwidth(bw), enter_time(e_time), send_time(s_time), start_time(-1), port_id(-1) {}
    bool operator < (const Flow& other) const {
        return enter_time > other.enter_time || (enter_time == other.enter_time && id > other.id);
    }
};

class Port {
public:
    int id, bandwidth;
    priority_queue<int, vector<int>, greater<int>> send_time_heap;
    Flow* current_flow;
    Port(int p_id, int bw) : id(p_id), bandwidth(bw), current_flow(nullptr) {}
    bool operator < (const Port& other) const {
        return id > other.id;
    }
};

vector<Port*> read_port_info(string port_file) {
    vector<Port*> port_list;
    ifstream ifs(port_file);
    string line;
    getline(ifs, line); // ignore the first line
    while (getline(ifs, line)) {
        int port_id, bandwidth;
        sscanf(line.c_str(), "%d,%d", &port_id, &bandwidth);
        port_list.push_back(new Port(port_id, bandwidth));
    }
    return port_list;
}

vector<Flow*> read_flow_info(string flow_file) {
    vector<Flow*> flow_list;
    ifstream ifs(flow_file);
    string line;
    getline(ifs, line); // ignore the first line
    while (getline(ifs, line)) {
        int flow_id, bandwidth, enter_time, send_time;
        sscanf(line.c_str(), "%d,%d,%d,%d", &flow_id, &bandwidth, &enter_time, &send_time);
        flow_list.push_back(new Flow(flow_id, bandwidth, enter_time, send_time));
    }
    return flow_list;
}

void send_flow(Port* port, Flow* flow) {
    port->send_time_heap.push(flow->send_time);
    flow->start_time = max(port->send_time_heap.top(), flow->enter_time);
    port->current_flow = flow;
    flow->port_id = port->id;
}

void optimize_bandwidth(vector<Port*>& port_list, vector<Flow*>& flow_list) {
    priority_queue<Flow*> flow_heap;
    for (Flow* flow : flow_list) {
        flow_heap.push(flow);
    }
    while (!flow_heap.empty()) {
        Flow* flow = flow_heap.top();
        flow_heap.pop();
        int bandwidth = flow->bandwidth;
        bool assigned = false;
        for (Port* port : port_list) {
            if (port->bandwidth >= bandwidth && port->send_time_heap.empty()) {
                send_flow(port, flow);
                assigned = true;
                break;
            }
        }
        if (!assigned) {
            for (Port* port : port_list) {
                if (port->current_flow != nullptr && 
                    port->bandwidth >= bandwidth && 
                    port->send_time_heap.top() <= flow->enter_time) {
                    Flow* prev_flow = port->current_flow;
                    port->current_flow = nullptr;
                    send_flow(port, flow);
                    flow_heap.push(prev_flow);
                    assigned = true;
                    break;
                }
            }
        }
        if (!assigned) {
            for (Port* port : port_list) {

                if (port->bandwidth >= bandwidth) {
                    int available_bandwidth = port->bandwidth;
                    priority_queue<int, vector<int>, greater<int>> temp_heap = port->send_time_heap;
                    while (!temp_heap.empty() && temp_heap.top() <= flow->enter_time) {
                        temp_heap.pop();
                        available_bandwidth += bandwidth;
                    }
                    if (available_bandwidth >= bandwidth) {
                        send_flow(port, flow);
                        while (!temp_heap.empty()) {
                            int send_time = temp_heap.top();
                            temp_heap.pop();
                            Flow* prev_flow = port->current_flow;
                            port->current_flow = nullptr;
                            for (Flow* f : flow_heap) {
                                if (f->id == prev_flow->id) {
                                    flow_heap.erase(f);
                                    break;
                                }
                            }
                            flow_heap.push(prev_flow);
                            send_flow(port, prev_flow);
                        }
                        assigned = true;
                        break;
                    }
                }
        }
        if (!assigned) {
            flow_heap.push(flow);
        }
    }
}

int main() {
    vector<Port*> port_list = read_port_info("port.txt");
    vector<Flow*> flow_list = read_flow_info("flow.txt");
    optimize_bandwidth(port_list, flow_list);
    ofstream ofs("reslut.txt");
    for (Flow* flow : flow_list) {
        ofs << flow->id << "," << flow->port_id << "," << flow->start_time << endl;
    }
    for (Port* port : port_list) {
        delete port;
    }
    for (Flow* flow : flow_list) {
        delete flow;
    }
    return 0;
}
