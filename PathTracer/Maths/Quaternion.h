#ifndef __MATHS_QUATERNION_H
#define __MATHS_QUATERNION_H
/**
* @file Quaternion.h
* @author Ewan Burnett (EwanBurnettSK@outlook.com)
* @date 2024-07-21
*/
#include "Vector3.h"
#include "Vector4.h"

namespace EDX {

    namespace Maths {
        template<typename T>
        struct Matrix4x4;

        template<typename T>
        struct Vector4;

        class Quaternion {
        public:

            double w;
            Vector3<double> v;

            Quaternion() : w(1.0), v(0.0, 0.0, 0.0) {  };
            Quaternion(double w, double x, double y, double z) : w(w), v({ x, y, z }) {};

            template<typename T>
            Quaternion(double w, Vector3<T> v) : w(w), v(v) {};

            template<typename T>
            Quaternion(Vector4<T> v) : w(v.w), v({ v.x, v.y, v.x }) {};


            inline friend Quaternion operator *(const Quaternion& lhs, const Quaternion& rhs) {
                Vector3d _v = Vector3<double>::Cross(lhs.v, rhs.v) + (rhs.w * lhs.v) + (lhs.w * rhs.v);// -(Vector3<double>::Dot(v, rhs.v)));
                double _w = (lhs.w * rhs.w) - Vector3<double>::Dot(lhs.v, rhs.v);

                return { _w, _v };
            }

            template<typename T>
            inline friend Vector3<T> operator * (const Vector3<T>& lhs, const Quaternion& rhs) {
                Quaternion p(0.0, lhs);  //Load the Vector into a Quaternion

                return (rhs * p * rhs.Conjugate()).v;    //As a rotation quaternion, the conjugate is the inverse. 
            }

            template<typename T>
            inline friend Vector3<T> operator * (const Quaternion& lhs, const Vector3<T>& rhs) {
                Quaternion p(0.0, rhs);  //Load the Vector into a Quaternion

                return (lhs * p * lhs.Conjugate()).v;    //As a rotation quaternion, the conjugate is the inverse. 
            }

            template<typename T>
            inline friend Vector4<T> operator * (const Vector4<T>& lhs, const Quaternion& rhs) {
                Quaternion p(0.0, Vector3<T>{lhs.x, lhs.y, lhs.z});  //Load the Vector into a Quaternion

                return (rhs * p * rhs.Conjugate()).v;    //As a rotation quaternion, the conjugate is the inverse. 
            }

            template<typename T>
            inline friend Vector4<T> operator * (const Quaternion& lhs, const Vector4<T>& rhs) {
                Quaternion p(0.0, Vector3<T>{ rhs.x, rhs.y, rhs.z });  //Load the Vector into a Quaternion

                return (lhs * p * lhs.Conjugate()).v;    //As a rotation quaternion, the conjugate is the inverse. 
            }


            template<typename T, typename std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
            inline friend Quaternion operator *(const Quaternion& lhs, const T& rhs) {

                return { Quaternion(rhs, Vector3<T>{0.0, 0.0, 0.0})* lhs };
            }


            template<typename T, typename std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
            inline friend Quaternion operator *(const T& lhs, const Quaternion& rhs) {
                return { Quaternion(lhs, Vector3<T>{0.0, 0.0, 0.0})* rhs };
            }



            inline Quaternion operator +(const Quaternion& rhs) {
                return { w + rhs.w, v + rhs.v };
            }

            /**
             * @brief Computes the Conjugate of this Quaternion.
             * @return
            */
            inline Quaternion Conjugate() const {
                return { w, -v };
            }

            /**
             * @brief Computes the Conjugate of a given Quaternion.
             * @param q
             * @return
            */
            inline static Quaternion Conjugate(const Quaternion& q) {
                return { q.w, -q.v };
            }

            /**
             * @brief Computes the Norm of this Quaternion.
             * @return
            */
            double Norm() const {
                return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z) * (w * w));
            }

            /**
             * @brief Computes the Norm of a given Quaternion.
             * @param q
             * @return
            */
            inline static double Norm(const Quaternion& q) {
                return sqrt((q.v.x * q.v.x) + (q.v.y * q.v.y) + (q.v.z * q.v.z) * (q.w * q.w));
            }

            /**
             * @brief Computes the Inverse of this Quaternion.
             * @return
            */
            inline Quaternion Inverse() const {
                double n = Norm();
                n = n * n;
                return ((1.0 / n) * Conjugate());
            }

            /**
             * @brief Computes the Inverse of a given Quaternion.
             * @param q
             * @return
            */
            inline static Quaternion Inverse(const Quaternion& q) {
                double n = Norm(q);
                n = n * n;
                return ((1.0 / n) * Conjugate(q));
            }

            /**
             * @brief Returns the Quaternion Identity.
            */
            inline static Quaternion Identity() {
                return{ 1.0, Vector3<double>{0.0, 0.0, 0.0} };
            }

            bool operator == (Quaternion rhs) { return w == rhs.w && v == rhs.v; };
            bool operator != (Quaternion rhs) { return !(*this == rhs); };

          
            template<typename T>
            inline static Vector3<T> RotateVector(const Vector3<T> vector, const Vector3<T> axis, const double radians) {
                const double r = radians / 2.0;
                const double sinTheta = sin(r);
                const double cosTheta = cos(r);
                Quaternion q = Quaternion::FromAxisAngle(axis, radians);  //Construct a unit quaternion from our rotation
                Quaternion p(0.0, vector);  //Load the Vector into a Quaternion

                return (q * p * Conjugate(q)).v;    //As a rotation quaternion, the conjugate is the inverse. 
            }


            template<typename T>
            inline static Vector4<T> ToAxisAngle(const Quaternion& q) {

                //In the case of the Quaternion Identity, 
                if (q.w == 1.0) {
                    return { 1.0, 0.0, 0.0, 0.0 };
                }

                //Extract the rotation angle from w
                double theta = 2.0 * (acos(q.w));

                // If theta is not 0, find the rotation axis unit vector
                if (theta != 0.0) {
                    return { q.v.x / sin(theta / 2.0), q.v.y / sin(theta / 2.0), q.v.z / sin(theta / 2.0), theta };
                }

                else {
                    return { 0.0, 0.0, 0.0, 0.0 };
                }
            }

            template<typename T>
            inline Vector4<T> ToAxisAngle() {
                return Quaternion::ToAxisAngle<T>(*this);
            }

            template<typename T>
            inline static Quaternion FromAxisAngle(const Vector3<T> axis, const double angleRadians) {
                const double r = angleRadians / 2.0;
                const double sinTheta = sin(r);
                const double cosTheta = cos(r);
                return { cosTheta, (Vector3<T>::Normalize(axis) * sinTheta) };
            }

            /**
             * @brief Performs Linear Interpolation between two quaternions
             * @param a
             * @param b
             * @param t
             * @return
            */
            inline static Quaternion Lerp(const Quaternion& a, const Quaternion& b, const double t) {
                Quaternion q = {};

                q = ((1.0 - t) * a) + b;
                q = q * (1.0 / q.Norm());

                return q;
            }

            /**
             * @brief Performs Spherical Linear Interpolation between two unit Quaternions.
             * @param a
             * @param b
             * @param t Interpolation constant, where 0 <= t <= 1
             * @return
            */
            inline static Quaternion Slerp(const Quaternion& a, const Quaternion& b, const double t) {
                double phi = acos((a.v.x * b.v.x) + (a.v.y * b.v.y) + (a.v.z * b.v.z) * (a.w * b.w));
                return ((sin(phi * (1.0 - t)) / sin(phi)) * a) + ((sin(phi * t) / sin(phi)) * b);
            }

            static Quaternion FromMatrix4x4(const Matrix4x4<double>& mat);
            Matrix4x4<float> ToMatrix4x4();
            static Matrix4x4<float> ToMatrix4x4(const Quaternion& q);


            //From Yaw-Pitch-Roll (z, y, x)
            //To Pitch-Yaw-Roll (x, y, z)
            // x -> y, y -> x, z -> z
            // x -> y, y -> x, z -> z
            //Roll = z, Pitch = x, Yaw = y
            template<typename T>
            static Vector3<T> ToEulerAngles(const Quaternion& q) {


                double roll = atan2(2.0 * ((q.w * q.v.x) + (q.v.y * q.v.z)), ((q.w * q.w) - (q.v.x * q.v.x) - (q.v.y * q.v.y) + (q.v.z * q.v.z)));
                double pitch = asin(2.0 * ((q.w * q.v.y) - (q.v.x * q.v.z)));
                double yaw = atan2(2.0 * ((q.w * q.v.z) + (q.v.x * q.v.y)), ((q.w * q.w) + (q.v.x * q.v.x) - (q.v.y * q.v.y) - (q.v.z * q.v.z)));


                //Resolve Gimbal Lock
                if (pitch == PI / 2.0) {
                    yaw = -2.0 * atan2(q.v.x, q.w);
                }
                else if (pitch == -(PI / 2.0)) {
                    yaw = 2.0 * atan2(q.v.x, q.w);
                }

                return { static_cast<T>(roll), static_cast<T>(pitch), static_cast<T>(yaw) };
            }

            template<typename T>
            inline Vector3<T> ToEulerAngles() {
                return ToEulerAngles<T>(*this);
            }


            //Roll = z, Pitch = x, Yaw = y
            template<typename T>
            static Quaternion FromEulerAngles(const Vector3<T>& eulerDegrees) {

                //Convert the input angles to Radians, and halve.
                const auto radians = eulerDegrees * Deg2Rad * 0.5;

                //Precompute the conversion parameters
                const double cosRoll = cos(radians.x);
                const double sinRoll = sin(radians.x);

                const double cosPitch = cos(radians.y);
                const double sinPitch = sin(radians.y);

                const double cosYaw = cos(radians.z);
                const double sinYaw = sin(radians.z);


                //Construct the Quaternion
                Quaternion q = {};
                q.w = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw;
                q.v.x = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw;
                q.v.y = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw;
                q.v.z = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw;

                double n = q.Norm();

                return q;
            }
        };
    }
}

#endif