import heapq
import os

# 流信息
class Flow:
    def __init__(self, flow_id, bandwidth, enter_time, send_time):
        self.id = flow_id
        self.bandwidth = bandwidth
        self.enter_time = enter_time
        self.send_time = send_time
        self.start_time = -1
        self.port_id = -1

    def __lt__(self, other):
        return self.enter_time < other.enter_time or (self.enter_time == other.enter_time and self.id < other.id)


# 端口信息
class Port:
    def __init__(self, port_id, bandwidth):
        self.id = port_id
        self.bandwidth = bandwidth
        self.send_time_heap = []
        self.current_flow = None

    def __lt__(self, other):
        return self.id < other.id


# 读取端口信息
def read_port_info(port_file):
    port_list = []
    with open(port_file, encoding='gb18030', errors='ignore') as f:
        f.readline()
        #next(f)
        for line in f:
            items = line.strip().split(',')
            port_list.append(Port(int(items[0]), int(items[1])))
    return port_list


# 读取流信息
def read_flow_info(flow_file):
    flow_list = []
    with open(flow_file, encoding='gb18030', errors='ignore') as f:
        f.readline()
        #next(f)
        for line in f:
            items = line.strip().split(',')
            flow_list.append(Flow(int(items[0]), int(items[1]), int(items[2]), int(items[3])))
    return flow_list


# 主函数
def main():
    #挨个读取文件
    folder_path = '../data/'
    for subdir in os.listdir(folder_path):
        subdir_path = os.path.join(folder_path, subdir)



        # 读取输入文件
        port_list = read_port_info(os.path.join(subdir_path, 'port.txt'))
        flow_list = read_flow_info(os.path.join(subdir_path, 'flow.txt'))

        # 建立按进入时间排序的流信息堆
        flow_heap = []
        for flow in flow_list:
            heapq.heappush(flow_heap, flow)

        # 处理流信息
        while flow_heap:
            flow = heapq.heappop(flow_heap)
            best_port = None
            best_time = None

            # 找到当前流最早可用的端口
            for port in port_list:
                if port.bandwidth >= flow.bandwidth:
                    if len(port.send_time_heap) == 0 :
                        best_port = port
                        best_time = flow.enter_time
                        break
                else: continue

                send_Time, send_flow = port.send_time_heap[0]
                send_time = -send_Time

                if send_time < flow.enter_time:
                    if not best_port or send_time < best_time:
                        best_port = port
                        best_time = flow.enter_time
                    break
                send_bandwidth = sum(send_flow.bandwidth for (send_time, send_flow) in port.send_time_heap if -send_time >= flow.enter_time)
                if ((port.bandwidth - send_bandwidth) >= flow.bandwidth):
                    best_port = port
                    best_time = flow.enter_time
                    break
                else:
                    continue
            if not best_port:
                heapq.heappush(flow_heap, flow)
                continue

            # 更新端口信息
            heapq.heappush(best_port.send_time_heap, (-(best_time + flow.send_time), flow))
            flow.port_id = best_port.id
            flow.start_time = best_time

        # 输出结果
        with open(os.path.join(subdir_path, 'result.txt'), 'w') as f:
            f.write('流id,端口id,开始发送时间\n')
            for flow in sorted(filter(lambda f: f.start_time != -1, flow_list), key=lambda f: f.start_time):
                f.write(f'{flow.id},{flow.port_id},{flow.start_time}\n')


if __name__ == '__main__':
    main()
