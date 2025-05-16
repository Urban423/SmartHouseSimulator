#include "MotionSensor.h"

void MotionSenser::start() {
    auto [renderViews, size] = ECS::GetComponents<RenderView>();
    old_posituons.resize(size);
    for (int i = 0; i < size; i++) {
        old_posituons[i] = renderViews[i].object.transform.position;
    }
}

void MotionSenser::update() {
    auto [renderViews, size] = ECS::GetComponents<RenderView>();
    Vector3 pos1 = object.transform.position;
    float transform_rotatation_z = object.transform.rotation.z * 3.14f;
    float transform_rotatation_z_max = transform_rotatation_z + rotation_angle;
    float transform_rotatation_z_min = transform_rotatation_z - rotation_angle;

    for (int i = 0; i < size; i++) {
        Vector3 pos2 = renderViews[i].object.transform.position;
        Vector3 dir = pos2 - pos1;
        float rot = atan2(dir.y, dir.x);

        if (pos1 != pos2 && Vector3::SqrDistance(pos1, pos2) <= rudius * rudius &&
            transform_rotatation_z_min <= rot && rot <= transform_rotatation_z_max) {
            trigger = true;
        }

        old_posituons[i] = pos2;
    }
}