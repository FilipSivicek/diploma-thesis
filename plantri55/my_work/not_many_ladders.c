#pragma once
#include "../plantri.c"

#define FILTER not_many_ladders

static int not_many_ladders(int nbtot, int nbop, int doflip){
    int contains_four = 0;
    int two_ladders = 0;
    int three_ladders = 0;
    for (int i = 0; i < nv; i++){
        if (degree[i] == 4){
            contains_four = 1;
            EDGE *e = firstedge[i];
            int four_neigh = 0;
            for(int j = 0; j < 4; j++){
                four_neigh += (degree[e->end] == 4);
                e = e->next;
            }
            two_ladders += (four_neigh == 1);
            three_ladders += (four_neigh == 2);
        }
    }
    return (three_ladders <= 1) && (two_ladders <= 2) && contains_four;
}