#include <iostream>
#include <stdio.h>

using namespace std;

int  main() 
{
    int total_power = 0;
    int task_count[1000001] = {0}; // 时间片范围[0, 1000000]

    int n; // 任务数
    cin >> n;
    for (int i = 0; i < n; i++) {
        int start, end;
        cin >> start >> end;
        for (int j = start; j <= end; j++) {
            task_count[j]++;
        }
    }

    int state = 0; // 服务器状态，0表示空载，1表示单任务，2表示多任务
    int prev_count = 0; // 上一个时间片的任务数量
    for (int i = 0; i <= 1000000; i++) {
        int count = task_count[i]; // 当前时间片的任务数量
        if (count == 0) { // 空载状态
            if (state != 0) {
                total_power += 1; // 从非空载状态切换到空载状态，能耗+1
                state = 0;
            }
        } else if (count == 1) { // 单任务状态
            if (state == 0) {
                total_power += 1; // 从空载状态切换到单任务状态，能耗+1
            } else if (state == 2) {
                total_power += 3; // 从多任务状态切换到单任务状态，能耗+3
            }
            state = 1;
        } else { // 多任务状态
            if (state == 0) {
                total_power += 1; // 从空载状态切换到多任务状态，能耗+1
            } else if (state == 1) {
                total_power += 4; // 从单任务状态切换到多任务状态，能耗+4
            } else if (prev_count != count) {
                total_power += 2; // 多任务状态下，任务数量发生变化，能耗+2
            }
            state = 2;
        }
        prev_count = count;
    }

    cout << total_power << endl;

    return 0;
}
