#ifndef _BEAMPOSING_H_
#define _BEAMPOSING_H_

// ������Դ��������״�������Ѳ�λ�о� ��ѩ��
// TODO ��Ѳ�λ��Ѱ
#include <Eigen/Core>
#include <Eigen/Dense>
#include <vector>

// ���ø�˹ţ�ٷ��������Է���
float GaussianNewton(float thetaBp, float enta, unsigned int MainN, float sigma, unsigned int maxIter);

// ��Ѳ�λ���
std::vector<float> BosePosing(unsigned int Channels);

#endif