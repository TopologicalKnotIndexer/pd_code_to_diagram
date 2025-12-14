#pragma once

#include <cassert>

class IntMatrix {
private:
    int row, col;
    int **matrix;

public:
    ~IntMatrix() { // 析构的时候需要注意释放空间
        for(int i = 0; i < row; i += 1) {
            delete[] matrix[i];
        }
        delete[] matrix;
    }

    IntMatrix(const IntMatrix&) = delete;
    IntMatrix(IntMatrix&&) = delete;

    IntMatrix(int _row, int _col): row(_row), col(_col) {
        assert(row >= 1);
        assert(col >= 1);

        // 新申请一个二维数组
        matrix = new int* [row];
        for(int i = 0; i < row; i += 1) {
            matrix[i] = new int[col];
        }
        clear();
    }

    void clear() {
        for(int i = 0; i < row; i += 1) {
            for(int j = 0; j < col; j += 1) {
                matrix[i][j] = 0;
            }
        }
    }

    void set(int i, int j, int v) {
        assert(0 <= i && i < row);
        assert(0 <= j && j < col);
        matrix[i][j] = v;
    }

    int get(int i, int j) const {
        assert(0 <= i && i < row);
        assert(0 <= j && j < col);
        return matrix[i][j];
    }
};
