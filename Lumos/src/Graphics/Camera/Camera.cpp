#include "LM.h"
#include "Camera.h"
#include <imgui/imgui.h>

namespace lumos
{

	Camera::Camera(float FOV, float Near, float Far, float aspect)
		: m_Yaw(0.0f)
		, m_Pitch(0.0f)
		, m_AspectRatio(aspect)
		, m_Position(maths::Vector3(0.0f, 0.0f, 0.0f))
		, m_Velocity(maths::Vector3(0.0f))
		, m_RotateVelocity(maths::Vector2(0.0f))
		, m_ZoomVelocity(0.0f)
		, m_Fov(FOV)
		, m_Near(Near)
		, m_Far(Far)
	{
		Camera::BuildViewMatrix();
	};

	Camera::Camera(float pitch, float yaw, const maths::Vector3& position, float FOV, float Near, float Far, float aspect)
		: m_Yaw(yaw)
		, m_Pitch(pitch)
		, m_AspectRatio(aspect)
		, m_Position(position)
		, m_Velocity(maths::Vector3(0.0f))
		, m_RotateVelocity(maths::Vector2(0.0f))
		, m_ZoomVelocity(0.0f)
		, m_Fov(FOV)
		, m_Near(Near)
		, m_Far(Far)
	{
		Camera::BuildViewMatrix();
	}

	void Camera::UpdateScroll(float offset, float dt)
	{
		if (offset != 0.0f)
		{
			m_ZoomVelocity += dt * offset * 10.0f;
		}

		m_Position += GetForwardDirection() * m_ZoomVelocity;
		m_ZoomVelocity = m_ZoomVelocity * pow(m_ZoomDampeningFactor, dt);
	}

	void Camera::BuildViewMatrix()
	{
		m_ViewMatrix = maths::Matrix4::Rotation(-m_Pitch, maths::Vector3(1, 0, 0)) *
				maths::Matrix4::Rotation(-m_Yaw, maths::Vector3(0, 1, 0)) *
				maths::Matrix4::Translation(-m_Position);
	};

	maths::Matrix4 Camera::BuildViewMatrix(float pitch, float yaw) const
	{
		return	maths::Matrix4::Rotation(-pitch, maths::Vector3(1, 0, 0)) *
				maths::Matrix4::Rotation(-yaw, maths::Vector3(0, 1, 0)) *
				maths::Matrix4::Translation(-m_Position);
	};

	void Camera::UpdateProjectionMatrix(float width, float height)
	{
		m_ProjMatrix = maths::Matrix4::Perspective(m_Near, m_Far, width/height, m_Fov);
	}

	maths::Vector3 Camera::GetUpDirection() const
	{
		maths::Vector3 up = maths::Vector3(0, 1, 0);
		maths::Quaternion::RotatePointByQuaternion(GetOrientation(), up);
		return up;
	}

	maths::Vector3 Camera::GetRightDirection() const
	{
		maths::Vector3 right = maths::Vector3(1, 0, 0);
		maths::Quaternion::RotatePointByQuaternion(GetOrientation(), right);
		return right;
	}

	maths::Vector3 Camera::GetForwardDirection() const
	{
		maths::Vector3 forward = maths::Vector3(0, 0, -1);
		maths::Quaternion::RotatePointByQuaternion(GetOrientation(), forward);
		return forward;
	}

	maths::Vector3 Camera::CalculatePosition() const
	{
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}

	maths::Quaternion Camera::GetOrientation() const
	{
		return  maths::Quaternion::EulerAnglesToQuaternion(m_Pitch, m_Yaw, 1.0f);
	}

	void Camera::OnImGUI()
	{
			if (ImGui::TreeNode("Camera"))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
				ImGui::Columns(2);
				ImGui::Separator();

				ImGui::AlignTextToFramePadding();
				ImGui::Text("Position");
				ImGui::NextColumn();
				ImGui::PushItemWidth(-1);
				ImGui::DragFloat3("##Position", &m_Position.x);
				ImGui::PopItemWidth();
				ImGui::NextColumn();

				ImGui::AlignTextToFramePadding();
				ImGui::Text("Aspect");
				ImGui::NextColumn();
				ImGui::PushItemWidth(-1);
				ImGui::DragFloat("##Aspect", &m_AspectRatio);
				ImGui::PopItemWidth();
				ImGui::NextColumn();

				ImGui::AlignTextToFramePadding();
				ImGui::Text("Pitch");
				ImGui::NextColumn();
				ImGui::PushItemWidth(-1);
				ImGui::DragFloat("##Pitch", &m_Pitch);
				ImGui::PopItemWidth();
				ImGui::NextColumn();

				ImGui::AlignTextToFramePadding();
				ImGui::Text("Yaw");
				ImGui::NextColumn();
				ImGui::PushItemWidth(-1);
				ImGui::DragFloat3("##Yaw", &m_Yaw);
				ImGui::PopItemWidth();
				ImGui::NextColumn();

				ImGui::AlignTextToFramePadding();
				ImGui::Text("Fov");
				ImGui::NextColumn();
				ImGui::PushItemWidth(-1);
				ImGui::DragFloat("##Fov", &m_Fov);
				ImGui::PopItemWidth();
				ImGui::NextColumn();

				ImGui::AlignTextToFramePadding();
				ImGui::Text("Near");
				ImGui::NextColumn();
				ImGui::PushItemWidth(-1);
				ImGui::DragFloat("##Near", &m_Near);
				ImGui::PopItemWidth();
				ImGui::NextColumn();

				ImGui::AlignTextToFramePadding();
				ImGui::Text("Far");
				ImGui::NextColumn();
				ImGui::PushItemWidth(-1);
				ImGui::DragFloat("##Far", &m_Far);
				ImGui::PopItemWidth();
				ImGui::NextColumn();

				ImGui::Columns(1);
				ImGui::Separator();
				ImGui::PopStyleVar();

				ImGui::TreePop();
		}
	}
}
