#include "utilities/math.hpp"
#include "world.hpp"

#include <glm/gtx/rotate_vector.hpp>

#include <algorithm>
#include <iostream>

float world::Camera::get_hightmap_value(float x, float z) {
#ifdef RAIL_EDITOR_DEBUG
// std::cerr << "(x, z): (" << x << ", " << z << ") -> "
//           << "(" << (x / 20.0) + 0.5 << ", " << 1.0 - ((z / 20.0) + 0.5) << ")\n";
#endif
	return hm.get_value((x / 20.0) + 0.5, 1.0 - ((z / 20.0) + 0.5)) * 2;
}

void world::Camera::update_position() {
	focus_point.y = get_hightmap_value(focus_point.x, focus_point.z);

	glm::vec3 offset{0, 0, distance};
	offset = glm::rotate(offset, glm::radians(angle), glm::vec3{1, 0, 0});
	offset = glm::rotate(offset, glm::radians(direction), glm::vec3(0, 1, 0));

	cam_point = focus_point + offset;
	// This part doesn't work yet, disabling it for later
	// float height = get_hightmap_value(cam_point.x, cam_point.z);
	// if (cam_point.y < height) {
	// 	cam_point.y = height;
	// 	auto flat = cam_point;
	// 	flat.y = 0;

	// 	angle = glm::degrees(glm::dot(glm::normalize(cam_point), glm::normalize(flat)));
	// }
}

void world::Camera::update_matrix() {
	mat = glm::lookAt(cam_point, focus_point, glm::vec3{0, 1, 0});

	ready = true;
}

void world::Camera::slide(float forward, float right) {
	ready = false;

	glm::vec3 flat = cam_point - focus_point;
	flat.y = 0;

	focus_point += flat * forward;
	focus_point += glm::rotate(flat, glm::radians(90.0f), glm::vec3(0, 1, 0)) * right;

	update_position();
}

void world::Camera::rotate(float up, float right) {
	ready = false;

	angle -= up;
	angle = utilities::clamp(angle, -85.0f, 0.0f);
	direction -= right;

	update_position();
}

void world::Camera::zoom(float in) {
	ready = false;

	distance += in;
	distance = std::max(distance, 0.5f);

	update_position();
}
