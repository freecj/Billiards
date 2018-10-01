/*
 * Sphere.cpp
 */
#include "Sphere.h"
#include "Algebra.h"
void Sphere::draw() {
    glBegin(GL_TRIANGLES);

    for (float y = 0; y < m_segmentsY; ++y) {
        float lowerY = 0.5 - y / m_segmentsY;
        float upperY = 0.5 - (y+1.0) / m_segmentsY;
        float lowerR = transverseSliceRadiusSphere(0.5,lowerY);
        float upperR = transverseSliceRadiusSphere(0.5,upperY);
        for (int i = 0; i < m_segmentsX; ++i) {

            float lowerXL = ithSliceXComponent(lowerR,i,m_segmentsX);
            float upperXL = ithSliceXComponent(lowerR,i+1,m_segmentsX);
            float lowerZL = ithSliceZComponent(lowerR,i,m_segmentsX);
            float upperZL = ithSliceZComponent(lowerR,i+1,m_segmentsX);

            float lowerXU = ithSliceXComponent(upperR,i,m_segmentsX);
            float upperXU = ithSliceXComponent(upperR,i+1,m_segmentsX);
            float lowerZU = ithSliceZComponent(upperR,i,m_segmentsX);
            float upperZU = ithSliceZComponent(upperR,i+1,m_segmentsX);

            Vector norm1(lowerXL,lowerY,lowerZL);
            norm1.normalize();
            glNormal3f(norm1[0],norm1[1],norm1[2]);
            glVertex3f(lowerXL,lowerY,lowerZL);
			double tx1 = atan2(norm1[0], norm1[2]) / (2. * PI) + 0.5;
			double ty1 = asin(norm1[1]) / PI + .5;
			
			glTexCoord2f(tx1, ty1);
		

            Vector norm2(upperXL,lowerY,upperZL);
            norm2.normalize();
            glNormal3f(norm2[0],norm2[1],norm2[2]);
            glVertex3f(upperXL,lowerY,upperZL);
			tx1 = atan2(norm2[0], norm2[2]) / (2. * PI) + 0.5;
			 ty1 = asin(norm2[1]) / PI + .5;
			 if (IN_RANGE(ty1, 0) || IN_RANGE(ty1, 1)) {
				 tx1 = .5;
			 }

			glTexCoord2f(tx1, ty1);

            Vector norm3(lowerXU,upperY,lowerZU);
            norm3.normalize();
            glNormal3f(norm3[0],norm3[1],norm3[2]);
            glVertex3f(lowerXU,upperY,lowerZU);
			tx1 = atan2(norm3[0], norm3[2]) / (2. * PI) + 0.5;
			ty1 = asin(norm3[1]) / PI + .5;
			glTexCoord2f(tx1, ty1);

            Vector norm4(lowerXU,upperY,lowerZU);
            norm4.normalize();
            glNormal3f(norm4[0],norm4[1],norm4[2]);
            glVertex3f(lowerXU,upperY,lowerZU);
			tx1 = atan2(norm4[0], norm4[2]) / (2. * PI) + 0.5;
			ty1 = asin(norm4[1]) / PI + .5;
			glTexCoord2f(tx1, ty1);

            Vector norm5(upperXL,lowerY,upperZL);
            norm5.normalize();
            glNormal3f(norm5[0],norm5[1],norm5[2]);
            glVertex3f(upperXL,lowerY,upperZL);
			tx1 = atan2(norm5[0], norm5[2]) / (2. * PI) + 0.5;
			ty1 = asin(norm5[1]) / PI + .5;
			glTexCoord2f(tx1, ty1);

            Vector norm6(upperXU,upperY,upperZU);
            norm6.normalize();
            glNormal3f(norm6[0],norm6[1],norm6[2]);
            glVertex3f(upperXU,upperY,upperZU);
			tx1 = atan2(norm6[0], norm6[2]) / (2. * PI) + 0.5;
			ty1 = asin(norm6[1]) / PI + .5;
			glTexCoord2f(tx1, ty1);
        }
    }   
    glEnd();
}

void Sphere::drawNormal() {
    glBegin(GL_LINES);
    for (float y = 0; y < m_segmentsY; ++y) {
        float Y = 0.5 - y / m_segmentsY;
        float R = transverseSliceRadiusSphere(0.5,Y);
        for (int i = 0; i < m_segmentsX; ++i) {
            Point p1(ithSliceXComponent(R,i,m_segmentsX),Y,ithSliceZComponent(R,i,m_segmentsX));
            Vector v(p1[0],p1[1],p1[2]);
            v.normalize();
            glVertex3f(p1[0],p1[1],p1[2]);
            Point p2 = p1 + v / 10;
            glVertex3f(p2[0],p2[1],p2[2]);
        }
    }
    glEnd();
}

double Sphere::Intersect(Point eyePointP, Vector rayV, Matrix transformMatrix) {

    Vector P = eyePointP - ORIGIN;
    Vector d = rayV;

//    std::cout << "P[0]" << P[0] << std::endl;
//    std::cout << "d[0]" << d[0] << std::endl;

    double a = dot(d,d);
    double b = 2.0 * dot(P, d);
    double c = dot(P, P) - 0.25; // 0.25 == r^2

    double det = SQR(b) - 4.0 * a * c;

    if (det >= 0) {
        double t = fmin((-b + sqrt(det)) / (2.0 * a), (-b - sqrt(det)) / (2.0 * a));
        if (t > 0) {
            return t;
        }
    }
    return DBL_MAX;
}

Vector Sphere::findIsectNormal(Point eyePoint, Vector ray, double dist) {
    Point p = eyePoint + dist * ray;
    Vector norm = p - ORIGIN;
    return norm;
}