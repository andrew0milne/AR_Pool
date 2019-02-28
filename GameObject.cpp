#include "GameObject.h"

#include "system\debug_log.h"

GameObject::GameObject()
{
	x_rot_ = 0.0f;
	y_rot_ = 0.0f;
	z_rot_ = 0.0f;
	x_angular_vel_ = 0.0f;
	y_angular_vel_ = 0.0f;
	z_angular_vel_ = 0.0f;
	scale_factor_ = 1.0f;
	mass_ = 0.0f;
	friction_ = 0.005f;

	velocity_ = gef::Vector4(0.0, 0.0, 0.0);

	model_scene_ = NULL;
}

// Initiliases the game object with its mesh loaded from a file
GameObject::GameObject(gef::Platform& platform_, const char* filename, Tag tag_)
{	
	x_rot_ = 0.0f;
	y_rot_ = 0.0f;
	z_rot_ = 0.0f;
	x_angular_vel_ = 0.0f; 
	y_angular_vel_ = 0.0f; 
	z_angular_vel_ = 0.0f;
	scale_factor_ = 1.0f;
	mass_ = 0.0f;
	friction_ = 0.005f;

	velocity_ = gef::Vector4(0.0, 0.0, 0.0);

	// create a new scene object and read in the data from the file
	// no meshes or materials are created yet
	// we're not making any assumptions about what the data may be loaded in for
	model_scene_ = new gef::Scene();
	model_scene_->ReadSceneFromFile(platform_, filename);

	// we do want to render the data stored in the scene file so lets create the materials from the material data present in the scene file
	model_scene_->CreateMaterials(platform_);

	this->set_mesh(GetFirstMesh(platform_, model_scene_));

	// Sets its colour based on its tag
	switch (tag_)
	{
	case CUE:
	{	
		mat_.set_colour(0xffaccae5);
		break; 
	}
	case RED:
	{
		mat_.set_colour(0xff0000ff);
		break;
	}
	case YELLOW:
	{
		mat_.set_colour(0xff00ffff);
		break;
	}
	case WHITE:
	{
		mat_.set_colour(0xffffffff);
		break;
	}
	case POCKET:
	{
		mat_.set_colour(0xff000000);
		break;
	}
	default:
		mat_.set_colour(0xffff0000);
		break;
	}

	this->tag_ = tag_;
	active_ = true;
}

GameObject::~GameObject()
{
	if (model_scene_ != NULL)
	{
		delete model_scene_;
		model_scene_ = NULL;
	}
}

// Intialiases the gameobject as a primitive cube or sphere
void GameObject::Init(PrimitiveBuilder* pb, UInt32 colour, bool cube)
{
	if (cube)
	{
		this->set_mesh(pb->GetDefaultCubeMesh());
	}
	else
	{
		this->set_mesh(pb->GetDefaultSphereMesh());
	}
	
	mat_.set_colour(colour);
	move_mat_.set_colour(0xffff0000);
	is_ball = true;
}

gef::Mesh* GameObject::GetFirstMesh(gef::Platform& platform_, gef::Scene* scene)
{
	gef::Mesh* mesh = NULL;

	if (scene)
	{
		// now check to see if there is any mesh data in the file, if so lets create a mesh from it
		if (scene->mesh_data.size() > 0)
		{
			mesh = model_scene_->CreateMesh(platform_, scene->mesh_data.front());
		}
	}

	is_ball = false;

	return mesh;
}

// Updates the gameobject based of the camera's position
void GameObject::CueUpdate(float delta_time_, gef::Matrix44 transform_)
{
	// Sets the position to the inverse of the markers transform
	gef::Matrix44 inverse_origin;
	inverse_origin.AffineInverse(transform_);
	position_ = inverse_origin.GetTranslation();

	// Calculates how far its moved since the last frame
	float dist_x_ = powf((position_.x() - previous_position.x()), 2.0f);
	float dist_y_ = powf((position_.y() - previous_position.y()), 2.0f);
	float dist_z_ = powf((position_.z() - previous_position.z()), 2.0f);

	// calculates it current velocity, based of how far its moved
	velocity_.set_x(sqrtf(dist_x_) / delta_time_);
	velocity_.set_y(sqrtf(dist_y_) / delta_time_);
	velocity_.set_z(sqrtf(dist_z_) / delta_time_);

	previous_position = position_;

	//Sets up the matrices
	scale_matrix_.Scale(scale_);
	x_rot_matrix_.RotationX(x_rot_);
	y_rot_matrix_.RotationY(y_rot_);
	z_rot_matrix_.RotationZ(z_rot_);

	// Offsets the ball from the camera
	gef::Matrix44 offset_;
	offset_.SetIdentity();
	offset_.SetTranslation(gef::Vector4(0.0f, -0.075f, -0.6f));

	// Sets the transform
	transform_matrix_ = scale_matrix_ * x_rot_matrix_ * y_rot_matrix_ * z_rot_matrix_ * offset_ * inverse_origin ;
	this->set_transform(transform_matrix_ * transform_); 

	// Updates the bounding box
	bounding_sphere_ = this->mesh()->bounding_sphere().Transform(transform_matrix_* transform_);
	float radius = (this->mesh()->aabb().max_vtx().x() - this->mesh()->aabb().min_vtx().x()) / 2.0f;
	bounding_sphere_.set_radius(radius * scale_factor_);
}

// Updates the Gameobject in world space
void GameObject::Update(float delta_time_)
{
	UpdatePosition(delta_time_);
	UpdateRotation(delta_time_);
	UpdateVelocity(delta_time_);
	
	scale_matrix_.Scale(scale_);
	x_rot_matrix_.RotationX(x_rot_);
	y_rot_matrix_.RotationY(y_rot_);
	z_rot_matrix_.RotationZ(z_rot_);

	transform_matrix_ = scale_matrix_ * x_rot_matrix_ * y_rot_matrix_ * z_rot_matrix_;

	transform_matrix_.SetTranslation(position_);

	this->set_transform(transform_matrix_);

	bounding_sphere_ = this->mesh()->bounding_sphere().Transform(transform_matrix_);
	float radius = (this->mesh()->aabb().max_vtx().x() - this->mesh()->aabb().min_vtx().x()) / 2.0f;
	bounding_sphere_.set_radius(radius * scale_factor_);
}

// Updates the Gameobject relative to a local origin (transform_)
void GameObject::Update(float delta_time_, gef::Matrix44 transform_)
{
	UpdatePosition(delta_time_);
	UpdateRotation(delta_time_);
	UpdateVelocity(delta_time_);

	scale_matrix_.Scale(scale_);
	x_rot_matrix_.RotationX(x_rot_);
	y_rot_matrix_.RotationY(y_rot_);
	z_rot_matrix_.RotationZ(z_rot_);

	transform_matrix_ = scale_matrix_ * x_rot_matrix_ * y_rot_matrix_ * z_rot_matrix_;

	transform_matrix_.SetTranslation(position_);

	this->set_transform(transform_matrix_* transform_);

	bounding_sphere_ = this->mesh()->bounding_sphere().Transform(transform_matrix_*transform_);
	float radius = (this->mesh()->aabb().max_vtx().x() - this->mesh()->aabb().min_vtx().x()) / 2.0f;
	bounding_sphere_.set_radius(radius * scale_factor_);
}

// Renders the gameobejct
void GameObject::Render(gef::Renderer3D* renderer_3d_)
{
	renderer_3d_->set_override_material(&mat_);
	renderer_3d_->DrawMesh(*this);
}

// Returns the bounding sphere
gef::Sphere GameObject::GetBoundingSphere()
{
	return bounding_sphere_;
}

// Sets if the gameobejct is active
void GameObject::SetActive(bool active)
{
	active_ = active;

	if (!active)
	{
		SetVelocity(0.0f, 0.0f);
	}
}

// Returns of the gameobject is active
bool GameObject::GetActive()
{
	return active_;
}

void GameObject::SetRotation(gef::Vector4 rot_)
{
	x_rot_ = gef::DegToRad(rot_.x());
	y_rot_ = gef::DegToRad(rot_.y());
	z_rot_ = gef::DegToRad(rot_.z());
}

void GameObject::SetPosition(gef::Vector4 pos_)
{
	position_ = pos_;
}

void GameObject::SetScale(double scl_)
{
	scale_factor_ = scl_;
	scale_ = gef::Vector4(scale_factor_, scale_factor_, scale_factor_);
}

// Updates the offset from the current local origin
void GameObject::SetLocalOriginOffset(gef::Matrix44 origin)
{
	local_origin_offset = origin;
}

// Updates the objects rotation based of its angular velocity
void GameObject::UpdateRotation(float delta_time_)
{
	x_rot_ += x_angular_vel_ * delta_time_;
	y_rot_ += y_angular_vel_ * delta_time_;
	z_rot_ += z_angular_vel_ * delta_time_;
}

// Updates the position based of the current velocity
void GameObject::UpdatePosition(float delta_time_)
{
	position_ += velocity_ * delta_time_;
}

// If the object is moving reduce its speed by the frictional force
void GameObject::UpdateVelocity(float delta_time_)
{
	if (GetSpeed() != 0.0f)
	{
		if (GetSpeed() < friction_)
		{
			SetVelocity(0.0f, 0.0f);
		}
		else
		{
			if (velocity_.x() > 0.0f)
			{
				velocity_.set_x(velocity_.x() - friction_ * delta_time_);
			}
			else if (velocity_.x() < 0.0f)
			{
				velocity_.set_x(velocity_.x() + friction_ * delta_time_);
			}

			if (velocity_.y() > 0.0f)
			{
				velocity_.set_y(velocity_.y() - friction_ * delta_time_);
			}
			else if (velocity_.y() < 0.0f)
			{
				velocity_.set_y(velocity_.y() + friction_ * delta_time_);
			}
		}		
	}
}

void GameObject::SetVelocity(gef::Vector4 vel_)
{
	velocity_ = vel_;
}

void GameObject::SetVelocity(float speed_, float angle_)
{
	//float angle_radiens_ = (angle_ * 3.141592f) / 180.0f;
	
	velocity_.set_x(speed_ * cosf(angle_));
	velocity_.set_y(speed_ * sinf(angle_));
}

void GameObject::SetAngularXVelocity(float x_)
{
	x_angular_vel_ = x_;
}

void GameObject::SetAngularYVelocity(float y_)
{
	y_angular_vel_ = y_;
}

void GameObject::SetAngularZVelocity(float z_)
{
	z_angular_vel_ = z_;
}

void GameObject::SetMass(float mass_)
{
	this->mass_ = mass_;
}

// Strores a ray between two points at a specific angle
void GameObject::SetRay(gef::Matrix44 left, gef::Matrix44 right, float a)
{
	left_point_ = left.GetTranslation();
	right_point_ = right.GetTranslation();
	SetVelocity(1.0f, a);
	angle = a;
}

void GameObject::InvertXVelocity()
{
	velocity_.set_x(velocity_.x() * -1.0);
}

void GameObject::InvertYVelocity()
{
	velocity_.set_y(velocity_.y() * -1.0);
}

gef::Vector4 GameObject::GetPosition()
{
	return position_;
}

float GameObject::GetMass()
{
	return mass_;
}

float GameObject::GetVelocityAngle()
{
	return gef::RadToDeg((atanf(velocity_.y() / velocity_.x())));
}

float GameObject::GetAngle()
{
	return angle;
}

float GameObject::GetSpeed()
{
	float x_sq_ = velocity_.x() * velocity_.x();
	float y_sq_ = velocity_.y() * velocity_.y();
	
	float speed_ = (sqrtf(x_sq_ + y_sq_));

	if (velocity_.x() < 0.0f)
	{
		speed_ *= -1.0f;
	}

	return speed_;
}

gef::Vector4 GameObject::GetVelocity()
{
	return velocity_;
}

float GameObject::GetScaleFactor()
{
	return scale_factor_;
}

gef::Matrix44 GameObject::GetLocalOriginOffset()
{
	return local_origin_offset;
}

gef::Vector4 GameObject::GetLeftPoint()
{
	return left_point_;
}

gef::Vector4 GameObject::GetRightPoint()
{
	return right_point_;
}