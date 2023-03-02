#include <L2.h>
#include <Architecture.h>
#include <vector>

namespace L2 {

    void initiate_color_order();

    L2::fence_node* remove_node(L2::Function* fn);

    bool bigger_node(L2::fence_node left, L2::fence_node right);

    void color_function(L2::Function* fn);

    void color_program(L2::Program &p);

}