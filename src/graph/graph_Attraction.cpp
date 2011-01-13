
#include "graph/graph_Attraction.h"

namespace graph {

Attraction::Attraction(Particle& a, Particle& b, float k, float distance_min)
    : a_(a), b_(b), k_(k), distance_min_(distance_min) {
    on_ = true;
    distance_min_squared_ = distance_min_ * distance_min_;
}

void Attraction::apply() {


}



}