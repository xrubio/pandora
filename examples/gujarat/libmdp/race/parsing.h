#include <iostream>
#include <vector>
#include <stdio.h>
#include <math.h>

#include "hash.h"

namespace ps {

void header(std::ostream&, size_t, size_t);
void trailer(std::ostream&);
void draw_filled_rectangle(std::ostream&, int, int, int, int, int, int, int);
void draw_thick_line(std::ostream&, float, float, int, int);
void draw_text(std::ostream&, float, float, float, int, char*);

};

const char Valid = 1;
const char Start = 2;
const char Goal = 4;
const char Invalid = 8;

namespace std {
    namespace tr1 {
        template<> struct hash<std::pair<int, int> > {
            public:
                size_t operator()(const std::pair<int, int> &p) const { return p.first ^ p.second; }
        };
    };
};

class grid_t {
    char *map_;
    int rows_;
    int cols_;
    std::vector<int> starts_;
    std::vector<int> goals_;

  public:
    grid_t() : map_(0), rows_(0), cols_(0) { }
    virtual ~grid_t() { delete[] map_; }

    int rows() const { return rows_; }
    int cols() const { return cols_; }
    const std::vector<int> starts() const { return starts_; }
    int start(size_t i) const { return starts_[i]; }
    const std::vector<int> goals() const { return goals_; }
    int goal(size_t i) const { return goals_[i]; }
    bool goal_pos(int x, int y) const {
        return (x >= 0) && (x < rows_) && (y >= 0) && (y < cols_) && (map_[x*cols_+y] & Goal);
    }
    bool valid_pos(int x, int y) const {
        return (x >= 0) && (x < rows_) && (y >= 0) && (y < cols_) && (map_[x*cols_+y] & Valid);
    }
    int valid_path(short x1, short y1, short x2, short y2, short &ox, short &oy) const {
        assert(valid_pos(x1, y1));
        int rv = 0, ix = 0, lx = 0, iy = 0, ly = 0;

        if( (x1 != x2) && (y1 != y2) ) {
            float m = (float)(y2 - y1) / (float)(x2 - x1);
            float b = (float)(y1*x2 - y2*x1) / (float)(x2 - x1);
            int inc = (x2 - x1 > 0 ? 1 : -1);
            for( float x = x1; (x != x2 + inc) && (rv == 0); x += inc ) {
                float y = m*x + b;
                lx = ix;
                ly = iy;
                ix = (int)x;
                iy = (int)floor(y + 0.5);
                if( !valid_pos(ix, iy) )
                    rv = 1;
                else if( goal_pos(ix, iy) )
                    rv = 2;
            }
        } else if( (x1 != x2) && (y1 == y2) ) {
            ly = iy = (int)y1;
            int inc = (x2 - x1 > 0 ? 1 : -1);
            for( float x = x1; (x != x2 + inc) && (rv == 0); x += inc ) {
                lx = ix;
                ix = (int)x;
                if( !valid_pos(ix, iy) )
                    rv = 1;
                else if( goal_pos(ix, iy) )
                    rv = 2;
            }
        } else if( (x1 == x2) && (y1 != y2) ) {
            lx = ix = (int)x1;
            int inc = (y2 - y1 > 0 ? 1 : -1);
            for( float y = y1; (y != y2 + inc) && (rv == 0); y += inc ) {
                ly = iy;
                iy = (int)y;
                if( !valid_pos(ix, iy) )
                    rv = 1;
                else if( goal_pos(ix, iy) )
                    rv = 2;
            }
        }

        if( rv == 1 ) {
            ox = lx;
            oy = ly;
        } else if( rv == 2 ) {
            ox = ix;
            oy = iy;
        }
        return rv;
    }
    void dump(std::ostream &os, bool postscript = false) const;
    void parse(std::ostream &os, FILE *is);
};

