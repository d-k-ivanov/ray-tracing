#include "AABB.h"

const AABB AABB::Empty    = AABB(Interval::Empty, Interval::Empty, Interval::Empty);
const AABB AABB::Universe = AABB(Interval::Universe, Interval::Universe, Interval::Universe);
