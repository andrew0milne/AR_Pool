#include "Table.h"

#include <system\debug_log.h>

Table::Table()
{
}

// Initialises the table with 6 walls and pockets
Table::Table(gef::Platform& platform_, PrimitiveBuilder* primitive_builder_)
{
	for (int i = 0; i < 6; i++)
	{
		walls_[i] = new GameObject();
		walls_[i]->Init(primitive_builder_, 0xff00ff00, true);

		pockets_[i] = new GameObject(platform_, "ball1.scn", POCKET);
		pockets_[i]->SetScale(0.0008);
		pockets_[i]->SetVelocity(0.0f, gef::DegToRad(0.0f));
		pockets_[i]->SetAngularZVelocity(0.0f);
	}

	wall_mat_.set_colour(0x00ff00ff);
	pocket_mat_.set_colour(0x000000ff);

	Init();
}

// Cleans up the table
Table::~Table()
{
	for (int i = 0; i < 6; i++)
	{
		delete walls_[i];
		walls_[i] = NULL;

		delete pockets_[i];
		pockets_[i] = NULL;
	}

	visible_markers_.clear();
}

// Initialises the tables values so it can be reset
void Table::Init()
{
	locked_ = false;
	for (int i = 0; i < 6; i++)
	{		
		pockets_[i]->SetPosition(gef::Vector4(0.0, 0.0, 0.0));
		pockets_[i]->SetRotation(gef::Vector4(0.0, 0.0, 0.0));
	}

	locked_ = false;
	locked_marker_id_ = 6;
	max_distance_ = 0.0f;
	marker_found_ = false;
	more_than_three_markers = false;
	marker_count_ = 0;
}

// Update the table
void Table::Update(float frame_time)
{
	// If the user has locked in the table
	if (locked_)
	{
		// Gets the markers transform, if it can be seen
		marker_found_ = false;
		if (sampleIsMarkerFound(locked_marker_id_))
		{
			sampleGetTransform(locked_marker_id_, &local_origin_);
			marker_found_ = true;
		}

		// Updates the walls and pockets
		for (int i = 0; i < visible_markers_.size(); i++)
		{		
			gef::Matrix44 transform = walls_[i]->GetLocalOriginOffset() * local_origin_;

			walls_[i]->set_transform(transform);

			pockets_[i]->Update(frame_time, local_origin_);
		}
	}
	// Making a custom table
	else
	{
		marker_count_ = 0;
		
		// Get the visiable marker positions and IDs
		for (int i = 0; i < 6; i++)
		{
			markers_[i].found_ = false;
			if (sampleIsMarkerFound(i))
			{
				sampleGetTransform(i, &markers_[i].transform_);
				markers_[i].found_ = true;
				markers_[i].id_ = i;
				marker_count_++;
			}
		}

		more_than_three_markers = false;
		if (marker_count_ > 2)
		{
			more_than_three_markers = true;

			// Sets the local origin to the marker with the lowest number
			int lowestMarkerNum = 0;
			for (int i = 0; i < 6; i++)
			{
				if (markers_[i].found_ == true)
				{
					local_origin_ = markers_[i].transform_;
					lowestMarkerNum = i;
					break;
				}
			}

			// Calculates all the markers local transforms, relative to the local origin
			for (int i = 0; i < 6; i++)
			{
				if (markers_[i].found_ == true)
				{
					markers_[i].local_origin_offset = GetLocalTransform(local_origin_, markers_[i].transform_);
				}
			}

			// Calculates the angle between each marker and the midpoint of all the markers
			for (int i = 0; i < 6; i++)
			{
				if (markers_[i].found_ == true)
				{
					markers_[i].angle_ = GetAngle(GetMidPointTransform(true), markers_[i].local_origin_offset.GetTranslation());
				}
			}

			// Orders the markers by their angle
			visible_markers_.clear();
			visible_markers_ = GetMarkerOrder();

			if (visible_markers_.size() > 1)
			{
				for (int i = 0; i < visible_markers_.size(); i++)
				{
					int num = 0;

					if (i + 1 < visible_markers_.size())
					{
						num = i + 1;
					}

					// Sets the wall's transoform to the midpoint of two markers
					gef::Matrix44 local_transform_ = GetMidPointTransform(visible_markers_[i].local_origin_offset, visible_markers_[num].local_origin_offset, walls_[i]);
					walls_[i]->SetLocalOriginOffset(local_transform_);
					walls_[i]->set_transform(local_transform_ * local_origin_);

					// Sets the transform of the pockets to the markers
					pockets_[i]->SetPosition(visible_markers_[i].local_origin_offset.GetTranslation());
					pockets_[i]->Update(frame_time, local_origin_);
				}
			}
		}
	}
}

// Renders the table
void Table::Render(gef::Renderer3D* renderer_3d_)
{
	for (int i = 0; i < visible_markers_.size(); i++)
	{
		walls_[i]->Render(renderer_3d_);
	}
	for (int i = 0; i < visible_markers_.size(); i++)
	{
		pockets_[i]->Render(renderer_3d_);
	}
}

// Calculates and returns the offset from the current local origin
gef::Matrix44 Table::GetLocalTransform(gef::Matrix44 origin, gef::Matrix44 matrix)
{
	gef::Matrix44 inv_transform_;
	inv_transform_.AffineInverse(origin);

	gef::Matrix44 local_transform = matrix * inv_transform_;

	return local_transform;
}

// Lock the table
void Table::Lock()
{
	locked_marker_id_ = 6;
	
	// Finds the the markers with the lowest ID
	for (int i = 0; i < visible_markers_.size(); i++)
	{
		if (visible_markers_[i].id_ < locked_marker_id_)
		{
			locked_marker_id_ = visible_markers_[i].id_;
		}
	}

	// Sets the local origin to the midpoint of all the markers
	gef::Matrix44 matrix = local_origin_;
	local_origin_.SetTranslation(GetMidPointTransform(false));

	// Calucaltes the maximum distance of the markers to the midpoint
	max_distance_ = GetMaxDistance();;

	for (int i = 0; i < visible_markers_.size(); i++)
	{
		int num = 0;
		
		if (i + 1 < visible_markers_.size())
		{
			num = i + 1;
		}
		
		// Update the wall and pocket's origin offsets based off the midpoint
		gef::Matrix44 pocket_transform_ = GetLocalTransform(local_origin_, pockets_[i]->transform());

		pockets_[i]->SetPosition(pocket_transform_.GetTranslation());
			
		walls_[i]->SetLocalOriginOffset(GetLocalTransform(local_origin_, walls_[i]->transform()));
		walls_[i]->SetRay
		(
			GetLocalTransform(local_origin_, visible_markers_[i].transform_), 
			GetLocalTransform(local_origin_, visible_markers_[num].transform_),
			GetAngle(visible_markers_[i].transform_.GetTranslation(), visible_markers_[num].transform_.GetTranslation())
		);
	}

	locked_ = true;
}

// Bubble sorts the markers based on their angle
std::vector<Markers> Table::GetMarkerOrder()
{
	std::vector<Markers> found_markers_;

	for (int i = 0; i < 6; i++)
	{
		if (markers_[i].found_ == true)
		{
			found_markers_.push_back(markers_[i]);
		}
	}

	for (int j = 1; j < found_markers_.size(); j++)
	{
		for (int i = 0; i < found_markers_.size() - j; i++)
		{
			if (found_markers_[i].angle_ > found_markers_[i + 1].angle_)
			{
				Markers temp = found_markers_[i];
				found_markers_[i] = found_markers_[i + 1];
				found_markers_[i + 1] = temp;
			}
		}
	}

	return found_markers_;
}

// A = mid point, C = card, a = angle betwwen A & C
// 
//		|   C
//		|  /
//		|a/
//		|/
//		A
//
//
float Table::GetAngle(gef::Vector4 A, gef::Vector4 C)
{
	gef::Vector4 B = A;
	B.set_y(B.y() + 1.0f);

	float a_ = GetDistance(B, C);
	float b_ = GetDistance(C, A);
	float c_ = GetDistance(A, B);

	float angle_A_ = ((b_ * b_) + (c_ * c_) - (a_ * a_)) / (2 * b_ * c_);
	angle_A_ = acosf(angle_A_);

	angle_A_ = gef::RadToDeg(angle_A_);

	if (A.x() - C.x() > 0.0f)
	{
		angle_A_ = 360.0f - angle_A_;
	}

	return angle_A_;
}

// Calculates the mean position of all the markers
gef::Vector4 Table::GetMidPointTransform(bool local_space)
{
	std::vector<gef::Vector4> marker_positions_;
	
	gef::Vector4 midpoint_;

	midpoint_.set_x(0.0f);
	midpoint_.set_y(0.0f);
	midpoint_.set_z(0.0f);

	for (int i = 0; i < 6; i++)
	{
		if (markers_[i].found_ == true)
		{
			if (local_space)
			{
				marker_positions_.push_back(markers_[i].local_origin_offset.GetTranslation());
			}
			else
			{
				marker_positions_.push_back(markers_[i].transform_.GetTranslation());
			}
		}
	}

	for (int i = 0; i < marker_positions_.size(); i++)
	{
		midpoint_.set_x(midpoint_.x() + marker_positions_[i].x());
		midpoint_.set_y(midpoint_.y() + marker_positions_[i].y());
		midpoint_.set_z(midpoint_.z() + marker_positions_[i].z());
	}

	midpoint_.set_x(midpoint_.x() / marker_positions_.size());
	midpoint_.set_y(midpoint_.y() / marker_positions_.size());
	midpoint_.set_z(midpoint_.z() / marker_positions_.size());

	return midpoint_;
}

// Calculates the mean transform between two marker_0 and marker_1
gef::Matrix44 Table::GetMidPointTransform(gef::Matrix44 marker_transform_0, gef::Matrix44 marker_transform_1, GameObject* wall_)
{
	gef::Matrix44 marker_transform_01;
	gef::Matrix44 scale_matrix;
	gef::Matrix44 rotation_matrix;
	gef::Matrix44 final_transform;

	gef::Vector4 marker_0_position_ = marker_transform_0.GetTranslation();
	gef::Vector4 marker_1_position_ = marker_transform_1.GetTranslation();
	gef::Vector4 mid_point_01;

	// Distance between the markers
	float distance = GetDistance(marker_0_position_, marker_1_position_);

	// Scales in the Z by this distance
	scale_matrix.SetIdentity();
	scale_matrix.Scale(gef::Vector4(0.01f, distance, 0.02f));
	scale_matrix.SetTranslation(gef::Vector4(0.0f, 0.0f, 0.059f / 2.0f));

	// Gets the angle between the two markers
	float angle = atanf((marker_1_position_.x() - marker_0_position_.x()) / (marker_1_position_.y() - marker_0_position_.y()));

	// Get the midpoint between the two markers
	mid_point_01.set_x((marker_0_position_.x() + marker_1_position_.x()) / 2.0f);
	mid_point_01.set_y((marker_0_position_.y() + marker_1_position_.y()) / 2.0f);
	mid_point_01.set_z((marker_0_position_.z() + marker_1_position_.z()) / 2.0f);

	marker_transform_01.SetIdentity();
	marker_transform_01.SetTranslation(mid_point_01);

	rotation_matrix.SetIdentity();
	rotation_matrix.RotationZ(-angle);

	final_transform = scale_matrix * rotation_matrix * marker_transform_01;

	return final_transform;
}

int Table::NumberOfWalls()
{
	return visible_markers_.size();
}

// Calculates the pythagorian distance between pos_1 and pos_2
float Table::GetDistance(gef::Vector4 pos_1_, gef::Vector4 pos_2_)
{
	float dist_x_ = powf((pos_2_.x() - pos_1_.x()), 2.0f);
	float dist_y_ = powf((pos_2_.y() - pos_1_.y()), 2.0f);
	float dist_z_ = powf((pos_2_.z() - pos_1_.z()), 2.0f);

	float distance_ = dist_x_ + dist_y_ + dist_z_;

	return sqrtf(distance_);
}

gef::Matrix44 Table::GetLocalOrigin()
{
	return local_origin_;
}

// Sets a pockets transform based of a marker
gef::Matrix44 Table::SetUpPocket(gef::Matrix44 marker_transform)
{
	gef::Matrix44 transform_;
	gef::Matrix44 scale_matrix;
	gef::Matrix44 rotation_matrix;
	gef::Matrix44 final_transform;

	// Scales in the Z by this distance
	scale_matrix.SetIdentity();
	scale_matrix.Scale(gef::Vector4(0.08f, 0.08f, 0.08f));
	scale_matrix.SetTranslation(gef::Vector4(0.0f, 0.0f, 0.059f / 2.0f));

	rotation_matrix.SetIdentity();

	transform_.SetIdentity();
	transform_.SetTranslation(marker_transform.GetTranslation());

	final_transform = scale_matrix * rotation_matrix * transform_;

	return final_transform;
}

int Table::GetLockedMarkerID()
{
	return locked_marker_id_;
}

// Calculates the distances between all the pockets and the midpoint, returns the largest
float Table::GetMaxDistance()
{
	float dist = 0.0f;

	for (int i = 0; i < visible_markers_.size(); i++)
	{
		float d = GetDistance(visible_markers_[i].transform_.GetTranslation(), local_origin_.GetTranslation());

		if (d > dist)
		{
			dist = d;
		}
	}

	return dist;
}