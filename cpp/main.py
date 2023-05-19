import heapq
import os


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


class Port:
    def __init__(self, port_id, bandwidth):
        self.id = port_id
        self.bandwidth = bandwidth
        self.send_time_heap = []
        self.current_flow = None

    def __lt__(self, other):
        return self.id < other.id


def read_port_info(port_file):
    port_map = {}
    with open(port_file, encoding='gb18030', errors='ignore') as f:
        f.readline()
        for line in f:
            items = line.strip().split(',')
            port = Port(int(items[0]), int(items[1]))
            port.send_time_heap = []
            port_map[port.id] = port
    return port_map


def read_flow_info(flow_file):
    flow_list = []
    with open(flow_file, encoding='gb18030', errors='ignore') as f:
        f.readline()
        for line in f:
            items = line.strip().split(',')
            flow_list.append(Flow(int(items[0]), int(items[1]), int(items[2]), int(items[3])))
    return flow_list


def allocate_flow(port_map, flow):
    min_send_time = float('inf')
    selected_port = None
    for port_id, port in port_map.items():
        if port.current_flow is None:
            heapq.heappush(port.send_time_heap, flow.send_time)
            port.current_flow = flow
            flow.start_time = flow.enter_time
            flow.port_id = port.id
            return True
        elif port.bandwidth >= flow.bandwidth:
            if len(port.send_time_heap) == 0:
                finish_time = flow.send_time
            else:
                finish_time = heapq.heappop(port.send_time_heap)
            if finish_time <= flow.enter_time:
                port.current_flow = flow
                heapq.heappush(port.send_time_heap, flow.send_time)
                flow.start_time = flow.enter_time
                flow.port_id = port.id
                return True
            else:
                heapq.heappush(port.send_time_heap, finish_time)
                if finish_time < min_send_time:
                    min_send_time = finish_time
                    selected_port = port
    if selected_port is not None:
        selected_port.current_flow = flow
        heapq.heappush(selected_port.send_time_heap, flow.send_time)
        flow.start_time = min_send_time
        flow.port_id = selected_port.id
        return True
    return False


def main():
    folder_path = '../data/'
    for subdir in os.listdir(folder_path):
        subdir_path = os.path.join(folder_path, subdir)
        # 读取输入文件
        port_map = read_port_info(os.path.join(subdir_path, 'port.txt'))
        flow_list = read_flow_info(os.path.join(subdir_path, 'flow.txt'))

        for flow in flow_list:
            if not allocate_flow(port_map, flow):
                print("Error: No available port for flow {0}".format(flow.id))
            else:
                with open(os.path.join(subdir_path, 'result.txt'), 'w') as f:
                    f.write('流id,端口id,开始发送时间\n')
                    for flow in sorted(filter(lambda f: f.start_time != -1, flow_list), key=lambda f: f.start_time):
                        f.write(f'{flow.id},{flow.port_id},{flow.start_time}\n')

if __name__ == '__main__':
    main()

