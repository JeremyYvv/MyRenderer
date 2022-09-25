#include "tgaImage.h"
#include "model.h"
#include <memory>

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

	/*DrawLine(t1.x, t1.y, t2.x, t2.y, stImage, stColor);
	DrawLine(t1.x, t1.y, t3.x, t3.y, stImage, stColor);
	DrawLine(t2.x, t2.y, t3.x, t3.y, stImage, stColor);*/

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
bool IsPointInsideTriangle(Vec2i stPointP, Vec2i stVertexA, Vec2i stVertexB, Vec2i stVertexC)
{
    bool bIsInside = false;

    do
    {
        Vec2i stVecBP = stPointP - stVertexB;
        Vec2i stVecBA = stVertexA - stVertexB;
        Vec2i stVecAP = stPointP - stVertexA;
        Vec2i stVecAC = stVertexC - stVertexA;
        Vec2i stVecCP = stPointP - stVertexC;
        Vec2i stVecCB = stVertexB - stVertexC;

        // bp x ba
        int iNormal1 = stVecBP.x * stVecBA.y - stVecBP.y * stVecBA.x;
        int iNormal2 = stVecAP.x * stVecAC.y - stVecAP.y * stVecAC.x;
        int iNormal3 = stVecCP.x * stVecCB.y - stVecCP.y * stVecCB.x;

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
            if (IsPointInsideTriangle(stPointP, t1, t2, t3))
            {
                stImage.set(iDrawX, iDrawY, stColor);
            }
        }
    }
}

// ================= Lesson Tests ==================== //
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);

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

int main(int argc, char** argv) 
{
    Test1_DrawLines();

    Test2_DrawWireMesh();

    Test3_DrawPolygon_Old();

    Test4_DrawPolygon_New();

	return 0;
}
