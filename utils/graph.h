/******************************************************************************
* Copyright 2026 Universität Innsbruck
******************************************************************************/


#ifndef GRAPH_H
#define GRAPH_H

#pragma once

#include <vector>
#include <eigen3/unsupported/Eigen/MatrixFunctions>
#include <eigen3/Eigen/src/Eigenvalues/SelfAdjointEigenSolver.h>

using namespace std;
using namespace Eigen;
using DMatrix = Eigen::Matrix<double, Dynamic, Dynamic>;

class Graph
{
    DMatrix adjM_;
    int nodes_;
public:
    Graph(int nodes) { this->nodes_ = nodes; this->adjM_=DMatrix(nodes_, nodes_); }
    void insert_edge(long i, long j)
    {
        if(i == j) return;
        adjM_(i,j) = 1;
        adjM_(j,i) = 1;
    }

    vector<tuple<int,int>> getEdges()
    {
        vector<tuple<int,int>> listEdges;
        for(int row = 0; row < nodes_; row++)
            for (int col = 0; col < nodes_; col++)
                if (adjM_(row, col) == 1)
                    listEdges.push_back(make_tuple(row, col));
        return listEdges;    
    }
};

#endif