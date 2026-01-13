#pragma once

#include <cassert>
#include <vector>
#include <iomanip>
#include <iostream>

class IntMatrix {
private:
    std::vector<std::vector<int> > m_vec;
    const int m_row;
    const int m_col;

public:
    IntMatrix(int n, int m): m_row(n), m_col(m) {
        std::vector<int> one_line;
        for(int i = 0; i < m; i += 1) {
            one_line.push_back(0);
        }
        for(int i = 0; i < n; i += 1) {
            m_vec.push_back(one_line); // 拷贝构造
        }
    }

    int getPos(int i, int j) const {
        if(0 <= i && i < m_row && 0 <= j && j < m_col) {
            return m_vec[i][j];
        }else {
            return 0; // 出界的位置的值记为 0
        }
    }

    void setPos(int i, int j, int v) {
        assert(0 <= i && i < m_row && 0 <= j && j < m_col);
        m_vec[i][j] = v;
    }

    int getRowCnt() const {
        return m_row;
    }

    int getColCnt() const {
        return m_col;
    }

    virtual void outputValue(std::ostream& out, int val) const {
        out << std::setw(3) << std::setfill(' ') << val << " ";
    }

    void debugOutput(std::ostream& out, bool with_zero) const {
        for(int i = 0; i < m_row; i += 1) {
            for(int j = 0; j < m_col; j += 1) {
                if(getPos(i, j) != 0) {
                    outputValue(out, getPos(i, j));
                }else {
                    if(with_zero) {
                        outputValue(out, 0); // with_zero 模式下会输出边界的 0
                    }else {
                        out << "    ";
                    }
                }
            }
            out << std::endl;
        }
    }
};
