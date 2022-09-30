#include "tgaImage.h"
#include "model.h"
#include <memory>
#include <limits>

typedef struct tagPoint
{
	int x;
	int y;
} POINT;

void DrawLineAlongX(const POINT &stPointA, const POINT &stPointB, TGAImage &stImage, const TGAColor &stColor)
{
	// select start & end point (x value from small to large)
	POINT stStartPoint = stPointA.x <= stPointB.x ? stPointA : stPointB;
	POINT stEndPoint = stPointA.x <= stPointB.x ? stPointB : stPointA;

	// calculate line slope
	float fSlope = (float)(stEndPoint.y - stStartPoint.y) / (stEndPoint.x - stStartPoint.x);

	// calculat & draw every pixel along X axis
	for (int iDrawX = stStartPoint.x; iDrawX <= stEndPoint.x; ++iDrawX)
	{
		int iDrawY = fSlope * (iDrawX - stStartPoint.x) + stStartPoint.y;
		stImage.set(iDrawX, iDrawY, stColor);
	}
}

void DrawLineAlongY(const POINT& stPointA, const POINT& stPointB, TGAImage& stImage, const TGAColor& stColor)
{
	// select start & end point (y value from small to large)
	POINT stStartPoint = stPointA.y <= stPointB.y ? stPointA : stPointB;
	POINT stEndPoint = stPointA.y <= stPointB.y ? stPointB : stPointA;

	// calculate line slope
	float fSlope = (float)(stEndPoint.x - stStartPoint.x) / (stEndPoint.y - stStartPoint.y);

	// calculat & draw every pixel along Y axis
	for (int iDrawY = stStartPoint.y; iDrawY <= stEndPoint.y; ++iDrawY)
	{
		int iDrawX = fSlope * (iDrawY - stStartPoint.y) + stStartPoint.x;
		stImage.set(iDrawX, iDrawY, stColor);
	}
}

void DrawLine(int x1, int y1, int x2, int y2, TGAImage& stImage, const TGAColor& stColor)
{
	POINT stPointA = { 0 };
	stPointA.x = x1;
	stPointA.y = y1;
	POINT stPointB = { 0 };
	stPointB.x = x2;
	stPointB.y = y2;
	// draw along the longer axis
	if (std::abs(stPointA.x - stPointB.x) > std::abs(stPointA.y - stPointB.y))
	{
		DrawLineAlongX(stPointA, stPointB, stImage, stColor);
	}
	else
	{
		DrawLineAlongY(stPointA, stPointB, stImage, stColor);
	}
}

void DrawLine(Vec2i t1, Vec2i t2, TGAImage &stImage, const TGAColor &stColor)
{
    DrawLine(t1.x, t1.y, t2.x, t2.y, stImage, stColor);
}

void DrawTriangle_Old(Vec2i t1, Vec2i t2, Vec2i t3, TGAImage& stImage, const TGAColor& stColor)
{
	// draw triangles along y-axis
	// sort points by y ascending, for cutting the triangle to 2 parts
	POINT stPointTop = { 0 };
	POINT stPointMiddle = { 0 };
	POINT stPointBottom = { 0 };
	stPointTop.x = t1.x;
	stPointTop.y = t1.y;
	stPointMiddle.x = t2.x;
	stPointMiddle.y = t2.y;
	stPointBottom.x = t3.x;
	stPointBottom.y = t3.y;

	// bubble sort
	if (stPointMiddle.y > stPointTop.y)
	{
		std::swap(stPointMiddle, stPointTop);
	}
	if (stPointBottom.y > stPointTop.y)
	{
		std::swap(stPointBottom, stPointTop);
	}
	if (stPointBottom.y > stPointMiddle.y)
	{
		std::swap(stPointBottom, stPointMiddle);
	}

	// calcalute slope of line top-middle/top-bottom/middle-bottom
	float fSlopeTM = (float)(stPointTop.x - stPointMiddle.x) / (stPointTop.y - stPointMiddle.y);
	float fSlopeTB = (float)(stPointTop.x - stPointBottom.x) / (stPointTop.y - stPointBottom.y);
	float fSlopeMB = (float)(stPointMiddle.x - stPointBottom.x) / (stPointMiddle.y - stPointBottom.y);

	// draw stBottom ~ stMiddle part along y-axis from bottom to middle
	for (int iDrawY = stPointBottom.y; iDrawY <= stPointMiddle.y; ++iDrawY)
	{
		int iLeftDrawX = stPointBottom.x + fSlopeTB * (iDrawY - stPointBottom.y);
		int iRightDrawX = stPointBottom.x + fSlopeMB * (iDrawY - stPointBottom.y);
		DrawLine(iLeftDrawX, iDrawY, iRightDrawX, iDrawY, stImage, stColor);
	}

	// draw stMiddle ~ stTop part along y-axis from middle to top
	for (int iDrawY = stPointMiddle.y; iDrawY <= stPointTop.y; ++iDrawY)
	{
		int iLeftDrawX = stPointTop.x - fSlopeTB * (stPointTop.y - iDrawY);
		int iRightDrawX = stPointTop.x - fSlopeTM * (stPointTop.y - iDrawY);
		DrawLine(iLeftDrawX, iDrawY, iRightDrawX, iDrawY, stImage, stColor);
	}
}

// chech whether point P is inside triangle composed by point A/B/C
// method: by cross product
bool IsPointInsideTriangle(Vec2f stPointP, Vec2f stVertexA, Vec2f stVertexB, Vec2f stVertexC)
{
    bool bIsInside = false;

    do
    {
        Vec2f stVecBP = stPointP - stVertexB;
        Vec2f stVecBA = stVertexA - stVertexB;
        Vec2f stVecAP = stPointP - stVertexA;
        Vec2f stVecAC = stVertexC - stVertexA;
        Vec2f stVecCP = stPointP - stVertexC;
        Vec2f stVecCB = stVertexB - stVertexC;

        // bp x ba
        float iNormal1 = stVecBP.x * stVecBA.y - stVecBP.y * stVecBA.x;
        float iNormal2 = stVecAP.x * stVecAC.y - stVecAP.y * stVecAC.x;
        float iNormal3 = stVecCP.x * stVecCB.y - stVecCP.y * stVecCB.x;

        if (iNormal1 >= 0 && iNormal2 >= 0 && iNormal3 >= 0)
        {
            bIsInside = true;
        }
        if (iNormal1 < 0 && iNormal2 < 0 && iNormal3 < 0)
        {
            bIsInside = true;
        }

    } while (false);

    return bIsInside;
}

void DrawTriangle_New(Vec2i t1, Vec2i t2, Vec2i t3, TGAImage& stImage, const TGAColor& stColor)
{
    // get bounding box borders
    int iBoundLeft = std::min(std::min(t1.x, t2.x), t3.x);
    int iBoundRight = std::max(std::max(t1.x, t2.x), t3.x);
    int iBoundTop = std::max(std::max(t1.y, t2.y), t3.y);
    int iBoundBottom = std::min(std::min(t1.y, t2.y), t3.y);

    // check every pixel in bounding box, if in the triangle, draw it
    for (int iDrawY = iBoundTop; iDrawY >= iBoundBottom; --iDrawY)
    {
        for (int iDrawX = iBoundLeft; iDrawX <= iBoundRight; ++iDrawX)
        {
            Vec2i stPointP(iDrawX, iDrawY);
//            if (IsPointInsideTriangle(stPointP, t1, t2, t3))
//            {
//                stImage.set(iDrawX, iDrawY, stColor);
//            }
        }
    }
}

void DrawTriangleWitZBuffer(Vec3f v1, Vec3f v2, Vec3f v3, float *fZBuffer, TGAImage &stImage, const TGAColor &stColor)
{
    // get bounding box borders
    int iBoundLeft = std::min(std::min(v1.x, v2.x), v3.x);
    int iBoundRight = std::max(std::max(v1.x, v2.x), v3.x);
    int iBoundTop = std::max(std::max(v1.y, v2.y), v3.y);
    int iBoundBottom = std::min(std::min(v1.y, v2.y), v3.y);

    int iWidth = stImage.get_width();

//    a = ( (p2.y-p1.y)*(p3.z-p1.z)-(p2.z-p1.z)*(p3.y-p1.y) );

//    b = ( (p2.z-p1.z)*(p3.x-p1.x)-(p2.x-p1.x)*(p3.z-p1.z) );

//    c = ( (p2.x-p1.x)*(p3.y-p1.y)-(p2.y-p1.y)*(p3.x-p1.x) );

//    d = ( 0-(a*p1.x+b*p1.y+c*p1.z) );

    // planer equation: Ax + By + Cz + D = 0;
    double A = (v2.y - v1.y) * (v3.z - v1.z) - (v2.z - v1.z) * (v3.y - v1.y);
    double B = (v2.z - v1.z) * (v3.x - v1.x) - (v2.x - v1.x) * (v3.z - v1.z);
    double C = (v2.x - v1.x) * (v3.y - v1.y) - (v2.y - v1.y) * (v3.x - v3.x);
    double D = (0 - (A * v1.x + B * v1.y + C * v1.z));

    // check every pixel in bounding box, if in the triangle, draw it
    for (int iDrawY = iBoundTop; iDrawY >= iBoundBottom; --iDrawY)
    {
        for (int iDrawX = iBoundLeft; iDrawX <= iBoundRight; ++iDrawX)
        {
            Vec2f stPointP(iDrawX, iDrawY);
            double dDrawZ = (0 - (A * iDrawX + B * iDrawY + D)) / C;

            Vec2f stPoint1(v1.x, v1.y);
            Vec2f stPoint2(v2.x, v2.y);
            Vec2f stPoint3(v3.x, v3.y);
            if (IsPointInsideTriangle(stPointP, stPoint1, stPoint2, stPoint3) &&
                fZBuffer[iWidth * iDrawY + iDrawX] < dDrawZ)
            {
                fZBuffer[iWidth * iDrawY + iDrawX] = dDrawZ;
                stImage.set(iDrawX, iDrawY, stColor);
            }
        }
    }
}

// ================= Lesson Tests ==================== //
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0,255, 255);

void Test1_DrawLines(void)
{
	TGAImage image(100, 100, TGAImage::RGB);

	DrawLine(13, 20, 80, 40, image, white);
	DrawLine(80, 40, 13, 20, image, red);
	DrawLine(20, 13, 40, 80, image, red);
	DrawLine(20,  0, 20, 50, image, green);
	
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output_1.tga");
}

void Test2_DrawWireMesh(void)
{
	const int width = 800;
	const int height = 800;
	TGAImage image(width, height, TGAImage::RGB);

	std::shared_ptr<Model> pModel = std::make_shared<Model>("./african_head.obj");
	for (int i = 0; i < pModel->nfaces(); i++) {
		std::vector<int> face = pModel->face(i);
		for (int j = 0; j < 3; j++) {
			Vec3f v0 = pModel->vert(face[j]);
			Vec3f v1 = pModel->vert(face[(j + 1) % 3]);
			// vertext coordinate in [-1.0, 1.0], cange to [0, width/height]
			int x0 = (v0.x + 1.) * width / 2.;
			int y0 = (v0.y + 1.) * height / 2.;
			int x1 = (v1.x + 1.) * width / 2.;
			int y1 = (v1.y + 1.) * height / 2.;
			DrawLine(x0, y0, x1, y1, image, white);
		}
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output_2.tga");
}

void Test3_DrawPolygon_Old(void)
{
	const int width = 200;
	const int height = 200;
	TGAImage image(width, height, TGAImage::RGB);

	Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
	Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
	Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };
    DrawTriangle_Old(t0[0], t0[1], t0[2], image, red);
    DrawTriangle_Old(t1[0], t1[1], t1[2], image, white);
    DrawTriangle_Old(t2[0], t2[1], t2[2], image, green);

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output_3.tga");
}

void Test4_DrawPolygon_New(void)
{
    const int width = 200;
    const int height = 200;
    TGAImage image(width, height, TGAImage::RGB);

    Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
    Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
    Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };
    DrawTriangle_New(t0[0], t0[1], t0[2], image, red);
    DrawTriangle_New(t1[0], t1[1], t1[2], image, white);
    DrawTriangle_New(t2[0], t2[1], t2[2], image, green);

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output_4.tga");
}

void Test5_DrawFlatWithLight(void)
{
    const int width = 800;
    const int height = 800;
    TGAImage image(width, height, TGAImage::RGB);

    Vec3f light_dir(0,0,1); // define light_dir
    std::shared_ptr<Model> pModel = std::make_shared<Model>("./african_head.obj");
    for (int i = 0; i < pModel->nfaces(); i++) {
        std::vector<int> face = pModel->face(i);
        Vec2i screen_coords[3];
        Vec3f world_coords[3];
        for (int j=0; j<3; j++) {
            Vec3f v = pModel->vert(face[j]);
            screen_coords[j] = Vec2i((v.x+1.)*width/2., (v.y+1.)*height/2.);
            world_coords[j]  = v;
        }
        // cross product to get normal
        Vec3f n = (world_coords[1]-world_coords[0])^(world_coords[2]-world_coords[0]);
        n.normalize();
        float intensity = n*light_dir;
        if (intensity>0) {
            DrawTriangle_New(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(intensity*255, intensity*255, intensity*255, 255));
        }
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output_5.tga");
}

void Test6_DrawFlatWithZBuffer(void)
{
    const int width = 800;
    const int height = 800;
    TGAImage image(width, height, TGAImage::RGB);

    float *pZBuffer = new float[width * height];
    for (int i = 0; i < width * height; ++i)
    {
        pZBuffer[i] = std::numeric_limits<int>::min();
    }

    Vec3f light_dir(0, 0, 1); // define light_dir
    std::shared_ptr<Model> pModel = std::make_shared<Model>("./african_head.obj");
    for (int i = 0; i < pModel->nfaces(); i++)
    {
        std::vector<int> face = pModel->face(i);
        Vec3f screen_coords[3];
        Vec3f world_coords[3];
        for (int j = 0; j < 3; j++)
        {
            Vec3f v = pModel->vert(face[j]);
            // change range from [-1, 1] to [0, width or height]
            screen_coords[j] = Vec3f((v.x+1.) * width/2., (v.y+1.)*height/2., v.z);
            world_coords[j]  = v;
        }
        // cross product to get normal
        Vec3f n = (world_coords[1]-world_coords[0])^(world_coords[2]-world_coords[0]);
        n.normalize();
        float intensity = n * light_dir;
        if (intensity > 0)
        {
            DrawTriangleWitZBuffer(screen_coords[0], screen_coords[1], screen_coords[2],
                                   pZBuffer, image, TGAColor(intensity*255, intensity*255, intensity*255, 255));
        }
    }

    delete pZBuffer;
    pZBuffer = nullptr;

    image.flip_vertically();
    image.write_tga_file("output_6.tga");
}

int main(int argc, char** argv) 
{
//    Test1_DrawLines();

//    Test2_DrawWireMesh();

//    Test3_DrawPolygon_Old();

//    Test4_DrawPolygon_New();

//    Test5_DrawFlatWithLight();

    Test6_DrawFlatWithZBuffer();

	return 0;
}
