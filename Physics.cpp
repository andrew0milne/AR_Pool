#include "Physics.h"



Physics::Physics()
{
}


Physics::~Physics()
{
}

// Returns the pythagorian distance between pos_1_ and pos_2_
float Physics::GetDistance(gef::Vector4 pos_1_, gef::Vector4 pos_2_)
{
	float dist_x_ = powf((pos_2_.x() - pos_1_.x()), 2.0f);
	float dist_y_ = powf((pos_2_.y() - pos_1_.y()), 2.0f);
	float dist_z_ = powf((pos_2_.z() - pos_1_.z()), 2.0f);

	float distance_ = dist_x_ + dist_y_ + dist_z_;

	return sqrtf(distance_);
}

// Sphere To Ray collision
bool Physics::SphereToRayCollision(GameObject* ball_, GameObject* wall_, int i)
{
	gef::Sphere sphere_ = ball_->GetBoundingSphere();

	// Converts the corners of the wall to a ray
	// The ray's starting point
	float x0 = wall_->GetLeftPoint().x();
	float y0 = wall_->GetLeftPoint().y();

	// The ray's directions
	float xd = wall_->GetRightPoint().x() - x0;
	float yd = wall_->GetRightPoint().y() - y0;

	// Ball's position
	float xc = ball_->GetPosition().x();
	float yc = ball_->GetPosition().y();

	float sr = sphere_.radius();

	// Quadratic Equations
	float A = (xd * xd) + (yd * yd);
	float B = 2 * ((x0 * xd) + (y0 * yd) - (xd * xc) - (yd * yc));
	float C = ((x0 - xc) * (x0 - xc)) + ((y0 - yc) * (y0 - yc)) - (sr * sr);
	float D = (B*B) - 4 * A * C;

	// There is no collision
	if (D < 0.0f)
	{
		return false;
	}
	// There has been a collision
	else
	{
		float wall_angle = wall_->GetAngle();
		float a = wall_angle - 90.0f;
		
		float ball_angle = ball_->GetVelocityAngle();
		
		float c = 180.0f - a - ball_angle;

		float d = 180.0f - c - c;

		float f = d + c + a;

		f = 360.0f - f;

		ball_->SetVelocity(ball_->GetSpeed(), gef::DegToRad(f));

		

		/*float mass_1_ = ball_->GetMass();
		float mass_2_ = mass_1_;
		//float speed_1_ = ball_->GetSpeed();
		//float speed_2_ = speed_1_;


		//http://www.vobarian.com/collisions/2dcollisions2.pdf

		// Find the Normal vector between the two spheres
		gef::Vector4 normal_ = ball_->GetPosition() - wall_->transform().GetTranslation();
		gef::Vector4 unit_normal_ = gef::Vector4(0.0, 0.0, 0.0);

		float normal_magnitude_ = sqrtf((normal_.x() * normal_.x()) + (normal_.y() * normal_.y()));

		// Find the unit normal between the spheres
		unit_normal_.set_x(normal_.x() / normal_magnitude_);
		unit_normal_.set_y(normal_.y() / normal_magnitude_);

		// Find the unit tangent
		gef::Vector4 unit_tangent_ = gef::Vector4(unit_normal_.y() * -1.0, unit_normal_.x(), 0.0);

		// Project each spheres velocity vectors onto the unit and tangent normals
		float object_1_normal_ = DotProduct(ball_->GetVelocity(), unit_normal_);
		float object_1_tangent_ = DotProduct(ball_->GetVelocity(), unit_tangent_);

		float object_2_normal_ = DotProduct(wall_->GetVelocity(), unit_normal_);
		float object_2_tangent_ = DotProduct(wall_->GetVelocity(), unit_tangent_);

		// Find the new normal velocities
		float new_normal_velocity_1_ = (object_1_normal_ * (mass_1_ - mass_2_) + (2 * mass_2_ * object_2_normal_)) / (mass_1_ + mass_2_);
		float new_normal_velocity_2_ = (object_2_normal_ * (mass_2_ - mass_1_) + (2 * mass_1_ * object_1_normal_)) / (mass_1_ + mass_2_);

		// Convert the scaler normal and tangential velocities into vectors
		gef::Vector4 new_normal_vector_1_ = gef::Vector4(new_normal_velocity_1_ * unit_normal_.x(), new_normal_velocity_1_ * unit_normal_.y(), 0.0);
		gef::Vector4 new_normal_vector_2_ = gef::Vector4(new_normal_velocity_2_ * unit_normal_.x(), new_normal_velocity_2_ * unit_normal_.y(), 0.0);

		gef::Vector4 new_object_1_tangent_ = gef::Vector4(object_1_tangent_ * unit_tangent_.x(), object_1_tangent_ * unit_tangent_.y(), 0.0);
		gef::Vector4 new_object_2_tangent_ = gef::Vector4(object_2_tangent_ * unit_tangent_.x(), object_2_tangent_ * unit_tangent_.y(), 0.0);

		gef::Vector4 final_velocity_1_ = new_normal_vector_1_ + new_object_1_tangent_;
		gef::Vector4 final_velocity_2_ = new_normal_vector_2_ + new_object_2_tangent_;


		ball_->SetVelocity(final_velocity_1_);
		//object_2_->SetVelocity(final_velocity_2_);
		*/
		return true;
	}
}

// Sphere to sphere collision
bool Physics::SphereToSphereCollision(GameObject* object_1_, GameObject* object_2_, bool pocket)
{
	gef::Sphere sphere_1_ = object_1_->GetBoundingSphere();
	gef::Sphere sphere_2_ = object_2_->GetBoundingSphere();

	if (GetDistance(sphere_1_.position(), sphere_2_.position()) < (sphere_1_.radius() + sphere_2_.radius()))
	{
		if (pocket)
		{
			object_1_->SetActive(false);
		}
		else
		{
			float mass_1_ = object_1_->GetMass();
			float mass_2_ = object_2_->GetMass();
			float speed_1_ = object_1_->GetSpeed();
			float speed_2_ = object_2_->GetSpeed();


			//http://www.vobarian.com/collisions/2dcollisions2.pdf

			// Find the Normal vector between the two spheres
			gef::Vector4 normal_ = sphere_1_.position() - sphere_2_.position();
			gef::Vector4 unit_normal_ = gef::Vector4(0.0, 0.0, 0.0);

			float normal_magnitude_ = sqrtf((normal_.x() * normal_.x()) + (normal_.y() * normal_.y()));

			// Find the unit normal between the spheres
			unit_normal_.set_x(normal_.x() / normal_magnitude_);
			unit_normal_.set_y(normal_.y() / normal_magnitude_);

			// Find the unit tangent
			gef::Vector4 unit_tangent_ = gef::Vector4(unit_normal_.y() * -1.0, unit_normal_.x(), 0.0);

			// Project each spheres velocity vectors onto the unit and tangent normals
			float object_1_normal_ = DotProduct(object_1_->GetVelocity(), unit_normal_);
			float object_1_tangent_ = DotProduct(object_1_->GetVelocity(), unit_tangent_);

			float object_2_normal_ = DotProduct(object_2_->GetVelocity(), unit_normal_);
			float object_2_tangent_ = DotProduct(object_2_->GetVelocity(), unit_tangent_);

			// Find the new normal velocities
			float new_normal_velocity_1_ = (object_1_normal_ * (mass_1_ - mass_2_) + (2 * mass_2_ * object_2_normal_)) / (mass_1_ + mass_2_);
			float new_normal_velocity_2_ = (object_2_normal_ * (mass_2_ - mass_1_) + (2 * mass_1_ * object_1_normal_)) / (mass_1_ + mass_2_);

			// Convert the scaler normal and tangential velocities into vectors
			gef::Vector4 new_normal_vector_1_ = gef::Vector4(new_normal_velocity_1_ * unit_normal_.x(), new_normal_velocity_1_ * unit_normal_.y(), 0.0);
			gef::Vector4 new_normal_vector_2_ = gef::Vector4(new_normal_velocity_2_ * unit_normal_.x(), new_normal_velocity_2_ * unit_normal_.y(), 0.0);

			gef::Vector4 new_object_1_tangent_ = gef::Vector4(object_1_tangent_ * unit_tangent_.x(), object_1_tangent_ * unit_tangent_.y(), 0.0);
			gef::Vector4 new_object_2_tangent_ = gef::Vector4(object_2_tangent_ * unit_tangent_.x(), object_2_tangent_ * unit_tangent_.y(), 0.0);

			gef::Vector4 final_velocity_1_ = new_normal_vector_1_ + new_object_1_tangent_;
			gef::Vector4 final_velocity_2_ = new_normal_vector_2_ + new_object_2_tangent_;


			object_1_->SetVelocity(final_velocity_1_);
			object_2_->SetVelocity(final_velocity_2_);

			object_1_->UpdatePosition(0.02f);
			object_2_->UpdatePosition(0.02f);
			
		}

		return true;
	}
	else
	{
		return false;
	}
}

// Takes in two gef::vector4s and calculates the dot product of their x and y components
float Physics::DotProduct(gef::Vector4 vec_1_, gef::Vector4 vec_2_)
{
	float x_ = vec_1_.x() * vec_2_.x();
	float y_ = vec_1_.y() * vec_2_.y();

	return x_ + y_;
}
