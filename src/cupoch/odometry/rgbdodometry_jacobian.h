#pragma once

#include "cupoch/odometry/odometry_option.h"
#include "cupoch/utility/eigen.h"
#include "cupoch/utility/device_vector.h"

namespace cupoch {

namespace odometry {

typedef utility::device_vector<Eigen::Vector4i> CorrespondenceSetPixelWise;

/// Base class that computes Jacobian from two RGB-D images
class RGBDOdometryJacobian {
public:
    enum OdometryJacobianType {
        COLOR_TERM = 0,
        HYBRID_TERM = 1,
    };

    __host__ __device__
    RGBDOdometryJacobian(OdometryJacobianType jacobian_type) : jacobian_type_(jacobian_type) {}
    __host__ __device__
    virtual ~RGBDOdometryJacobian() {}

public:
    /// Function to compute i-th row of J and r
    /// the vector form of J_r is basically 6x1 matrix, but it can be
    /// easily extendable to 6xn matrix.
    /// See RGBDOdometryJacobianFromHybridTerm for this case.
    __host__ __device__
    virtual void ComputeJacobianAndResidual(
            int row,
            Eigen::Vector6f J_r[2],
            float r[2],
            const uint8_t* source_color,
            const uint8_t* source_depth,
            const uint8_t* target_color,
            const uint8_t* target_depth,
            const uint8_t* source_xyz,
            const uint8_t* target_dx_color,
            const uint8_t* target_dx_depth,
            const uint8_t* target_dy_color,
            const uint8_t* target_dy_depth,
            int width,
            const Eigen::Matrix3f &intrinsic,
            const Eigen::Matrix4f &extrinsic,
            const Eigen::Vector4i* corresps) const {};
    OdometryJacobianType jacobian_type_;
};

/// Class to compute Jacobian using color term
/// Energy: (I_p-I_q)^2
/// reference:
/// F. Steinbrucker, J. Sturm, and D. Cremers.
/// Real-time visual odometry from dense RGB-D images.
/// In ICCV Workshops, 2011.
class RGBDOdometryJacobianFromColorTerm : public RGBDOdometryJacobian {
public:
    __host__ __device__
    RGBDOdometryJacobianFromColorTerm() : RGBDOdometryJacobian(COLOR_TERM) {}
    __host__ __device__
    ~RGBDOdometryJacobianFromColorTerm() override {}

public:
    __host__ __device__
    void ComputeJacobianAndResidual(
            int row,
            Eigen::Vector6f J_r[2],
            float r[2],
            const uint8_t* source_color,
            const uint8_t* source_depth,
            const uint8_t* target_color,
            const uint8_t* target_depth,
            const uint8_t* source_xyz,
            const uint8_t* target_dx_color,
            const uint8_t* target_dx_depth,
            const uint8_t* target_dy_color,
            const uint8_t* target_dy_depth,
            int width,
            const Eigen::Matrix3f &intrinsic,
            const Eigen::Matrix4f &extrinsic,
            const Eigen::Vector4i* corresps) const override;
};

/// Class to compute Jacobian using hybrid term
/// Energy: (I_p-I_q)^2 + lambda(D_p-(D_q)')^2
/// reference:
/// J. Park, Q.-Y. Zhou, and V. Koltun
/// anonymous submission
class RGBDOdometryJacobianFromHybridTerm : public RGBDOdometryJacobian {
public:
    __host__ __device__
    RGBDOdometryJacobianFromHybridTerm() : RGBDOdometryJacobian(HYBRID_TERM) {}
    __host__ __device__
    ~RGBDOdometryJacobianFromHybridTerm() override {}

public:
    __host__ __device__
    void ComputeJacobianAndResidual(
            int row,
            Eigen::Vector6f J_r[2],
            float r[2],
            const uint8_t* source_color,
            const uint8_t* source_depth,
            const uint8_t* target_color,
            const uint8_t* target_depth,
            const uint8_t* source_xyz,
            const uint8_t* target_dx_color,
            const uint8_t* target_dx_depth,
            const uint8_t* target_dy_color,
            const uint8_t* target_dy_depth,
            int width,
            const Eigen::Matrix3f &intrinsic,
            const Eigen::Matrix4f &extrinsic,
            const Eigen::Vector4i* corresps) const override;
};

}  // namespace odometry
}  // namespace cupoch

#include "cupoch/odometry/rgbdodometry_jacobian.inl"