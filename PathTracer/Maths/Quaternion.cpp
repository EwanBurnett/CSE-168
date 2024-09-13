#include "Quaternion.h"
#include "Matrix.h"


EDX::Maths::Quaternion EDX::Maths::Quaternion::FromMatrix4x4(const Matrix4x4<double>& mat) {
    //TODO: 3x3 Matrix instead of 4x4!
    //Row 3 and column 3 are redundant.

    Quaternion out = {};

    //Find the Magnitude of each component of the Quaternion
    out.w = (1.0 + mat[0] + mat[5] + mat[10]) / 4.0;
    out.v.x = (1.0 + mat[0] - mat[5] - mat[10]) / 4.0;
    out.v.y = (1.0 - mat[0] + mat[5] - mat[10]) / 4.0;
    out.v.z = (1.0 - mat[0] - mat[5] + mat[10]) / 4.0;

    //Handle invalid numbers
    if (out.w < 0.0) {
        out.w = 0.0;
    }
    if (out.v.x < 0.0) {
        out.v.x = 0.0;
    }
    if (out.v.y < 0.0) {
        out.v.y = 0.0;
    }
    if (out.v.z < 0.0) {
        out.v.z = 0.0;
    }

    out.w = sqrt(out.w);
    out.v.x = sqrt(out.v.x);
    out.v.y = sqrt(out.v.y);
    out.v.z = sqrt(out.v.z);



    //Resolve the signs
    if (out.w >= out.v.x && out.w >= out.v.y && out.w >= out.v.z) {    //w is the largest
        out.v.x = (mat[9] - mat[6]) / (4.0 * out.w);
        out.v.y = (mat[2] - mat[8]) / (4.0 * out.w);
        out.v.z = (mat[4] - mat[1]) / (4.0 * out.w);
    }
    else if (out.v.x >= out.w && out.v.x >= out.v.y && out.v.z >= out.v.z) {    //x is the largest
        out.w = (mat[9] - mat[6]) / (4.0 * out.v.x);
        out.v.y = (mat[1] + mat[4]) / (4.0 * out.v.x);
        out.v.z = (mat[2] + mat[8]) / (4.0 * out.v.x);
    }
    else if (out.v.y >= out.w && out.v.y >= out.v.x && out.v.z >= out.v.z) {    //x is the largest
        out.w = (mat[2] - mat[8]) / (4.0 * out.v.y);
        out.v.x = (mat[1] + mat[4]) / (4.0 * out.v.y);
        out.v.z = (mat[6] + mat[9]) / (4.0 * out.v.y);
    }
    else if (out.v.z >= out.w && out.v.z >= out.v.y && out.v.z >= out.v.x) {    //x is the largest
        out.w = (mat[4] - mat[1]) / (4.0 * out.v.z);
        out.v.x = (mat[2] + mat[8]) / (4.0 * out.v.z);
        out.v.y = (mat[6] + mat[9]) / (4.0 * out.v.z);
    }
    else {
        //This shouldn't be possible!
        return {}; 
    }


    return out.Conjugate();
}

EDX::Maths::Matrix4x4<float> EDX::Maths::Quaternion::ToMatrix4x4() {
    return(Quaternion::ToMatrix4x4(*this));
}

EDX::Maths::Matrix4x4<float> EDX::Maths::Quaternion::ToMatrix4x4(const Quaternion& q) {

    Matrix4x4<float> out = {};

    const double w_2 = (q.w * q.w);
    const Vector3d v_2 = (q.v * q.v);

    /*
    out[0] = w_2 + v_2.x - v_2.y - v_2.z;
    out[1] = -((2.0 * q.v.x * q.v.y) - (2.0 * q.w * q.v.z));    //Negate some terms to ensure the same handedness. 
    out[2] = -((2.0 * q.v.x * q.v.z) + (2.0 * q.w * q.v.y));
    out[3] = 0.0;

    out[4] = -((2.0 * q.v.x * q.v.y) + (2.0 * q.w * q.v.z));
    out[5] = w_2 - v_2.x + v_2.y - v_2.z;
    out[6] = -((2.0 * q.v.y * q.v.z) - (2.0 * q.w * q.v.x));
    out[7] = 0.0;

    out[8] = -((2.0 * q.v.x * q.v.z) - (2.0 * q.w * q.v.y));
    out[9] = -((2.0 * q.v.y * q.v.z) + (2.0 * q.w * q.v.x));
    out[10] = (w_2 - v_2.x - v_2.y + v_2.z);
    out[11] = 0.0;

    out[12] = 0.0;
    out[13] = 0.0;
    out[14] = 0.0;
    out[15] = 1.0;
    */

    out[0] = w_2 + v_2.x - v_2.y - v_2.z;
    out[1] = ((2.0 * q.v.x * q.v.y) + (2.0 * q.w * q.v.z));    
    out[2] = ((2.0 * q.v.x * q.v.z) - (2.0 * q.w * q.v.y));
    out[3] = 0.0;

    out[4] = ((2.0 * q.v.x * q.v.y) - (2.0 * q.w * q.v.z));
    out[5] = w_2 - v_2.x + v_2.y - v_2.z;
    out[6] = ((2.0 * q.v.y * q.v.z) + (2.0 * q.w * q.v.x));
    out[7] = 0.0;

    out[8] = ((2.0 * q.v.x * q.v.z) + (2.0 * q.w * q.v.y));
    out[9] = ((2.0 * q.v.y * q.v.z) - (2.0 * q.w * q.v.x));
    out[10] = (w_2 - v_2.x - v_2.y + v_2.z);
    out[11] = 0.0;

    out[12] = 0.0;
    out[13] = 0.0;
    out[14] = 0.0;
    out[15] = 1.0;


    return out;
}

