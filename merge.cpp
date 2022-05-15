#include <queue>
#include <tuple>
#include "merge.h"

using namespace std;

using sort_data = tuple<int, int, int>;

struct sort_data_cmp {
    bool operator()(const sort_data &lhs, const sort_data &rhs) {
        return get<2>(lhs) > get<2>(rhs);
    }
};

void multimerge(int *starts[], const int lengths[], const int count, int *rslt,
                const int rslt_len) {
    priority_queue<sort_data, vector<sort_data>, sort_data_cmp> queue;
    for (int i = 0; i < count; i++) {
        if (lengths[i] > 0)
            queue.push(make_tuple(i, 0, starts[i][0]));
    }

    int rsi = 0;
    while (!queue.empty() && rsi < rslt_len) {
        int list, offset, val;
        tie(list, offset, val) = queue.top();
        queue.pop();
        rslt[rsi++] = val;
        if (lengths[list] > offset + 1) {
            queue.push(make_tuple(list, offset + 1, starts[list][offset + 1]));
        }
    }
}
