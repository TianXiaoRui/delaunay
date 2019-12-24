//
// Created by tr on 19-12-20.
//
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include<time.h>

using namespace std;
using namespace cv;
/*
pts，要剖分的散点集,in
img,剖分的画布,in
tri,存储三个表示顶点变换的正数,out
*/

// used for doing delaunay trianglation with opencv function
//该函数用来防止多次重画并消去虚拟三角形的 顶点
bool isGoodTri( Vec3i &v, vector<Vec3i> & tri )
{
    int a = v[0], b = v[1], c = v[2];
    v[0] = min(a,min(b,c));//v[0]找到点插入的先后顺序（0....N-1，N为点的个数）的最小值
    v[2] = max(a,max(b,c));//v[2]存储最大值.
    v[1] = a+b+c-v[0]-v[2];//v[1]为中间值
    if (v[0] == -1) return false;

    vector<Vec3i>::iterator iter = tri.begin();//开始时为空
    for(;iter!=tri.end();iter++)
    {
        Vec3i &check = *iter;//如果当前待压入的和存储的重复了，则停止返回false。
        if (check[0]==v[0] &&
            check[1]==v[1] &&
            check[2]==v[2])
        {
            break;
        }
    }
    if (iter == tri.end())
    {
        tri.push_back(v);
        return true;
    }
    return false;
}
/*
pts，要剖分的散点集,in
img,剖分的画布,in
tri,存储三个表示顶点变换的正数,out
*/
void TriSubDiv( vector<Point2f> &pts, Mat &img, vector<Vec3i> &tri )
{
    CvSubdiv2D* subdiv;
    CvMemStorage* storage = cvCreateMemStorage(0); //创建存储器
    Rect rc = Rect(0,0, img.cols, img.rows);//矩形是图像的大小

    subdiv = cvCreateSubdiv2D( CV_SEQ_KIND_SUBDIV2D, sizeof(*subdiv),
                               sizeof(CvSubdiv2DPoint),
                               sizeof(CvQuadEdge2D),
                               storage );//为剖分数据分配空间
    cvInitSubdivDelaunay2D( subdiv, rc );
    for (size_t i = 0; i < pts.size(); i++)
    {
        CvSubdiv2DPoint *pt = cvSubdivDelaunay2DInsert( subdiv, pts[i] );//利用插入法进行剖分
        pt->id = i;//为每一个顶点分配一个id

    }

    CvSeqReader reader;//利用CvSeqReader遍历
    int total = subdiv->edges->total;//边的总数
    int elem_size = subdiv->edges->elem_size;//边的大小

    cvStartReadSeq( (CvSeq*)(subdiv->edges), &reader, 0 );
    Point buf[3];
    const Point *pBuf = buf;
    Vec3i verticesIdx;
    Mat imgShow = img.clone();

    srand( (unsigned)time( NULL ) );
    for( int i = 0; i < total; i++ )
    {
        CvQuadEdge2D* edge = (CvQuadEdge2D*)(reader.ptr);

        if( CV_IS_SET_ELEM( edge ))
        {
            CvSubdiv2DEdge t = (CvSubdiv2DEdge)edge;
            int iPointNum = 3;
            Scalar color = CV_RGB(rand()&255,rand()&255,rand()&255);
            //Scalar color=CV_RGB(255,0,0);
            //bool isNeg = false;
            int j;
            for(j = 0; j < iPointNum; j++ )
            {
                CvSubdiv2DPoint* pt = cvSubdiv2DEdgeOrg( t );//获取t边的源点
                if( !pt ) break;
                buf[j] = pt->pt;//将点存储起来
                //if (pt->id == -1) isNeg = true;
                verticesIdx[j] = pt->id;//获取顶点的Id号，将三个点的id存储到verticesIdx中
                t = cvSubdiv2DGetEdge( t, CV_NEXT_AROUND_LEFT );//获取下一条边
            }
            if (j != iPointNum) continue;
            if (isGoodTri(verticesIdx, tri))
            {
                //tri.push_back(verticesIdx);
                polylines( imgShow, &pBuf, &iPointNum,
                           1, true, color,
                           1, CV_AA, 0);//画出三条边
                //printf("(%d, %d)-(%d, %d)-(%d, %d)\n", buf[0].x, buf[0].y, buf[1].x, buf[1].y, buf[2].x, buf[2].y);
                //printf("%d\t%d\t%d\n", verticesIdx[0], verticesIdx[1], verticesIdx[2]);
                //imshow("Delaunay", imgShow);
                //waitKey();
            }

            t = (CvSubdiv2DEdge)edge+2;//相反边缘 reversed e

            for(j = 0; j < iPointNum; j++ )
            {
                CvSubdiv2DPoint* pt = cvSubdiv2DEdgeOrg( t );
                if( !pt ) break;
                buf[j] = pt->pt;
                verticesIdx[j] = pt->id;
                t = cvSubdiv2DGetEdge( t, CV_NEXT_AROUND_LEFT );
            }
            if (j != iPointNum) continue;
            if (isGoodTri(verticesIdx, tri))
            {
                //tri.push_back(verticesIdx);
                polylines( imgShow, &pBuf, &iPointNum,
                           1, true, color,
                           1, CV_AA, 0);
                //printf("(%d, %d)-(%d, %d)-(%d, %d)\n", buf[0].x, buf[0].y, buf[1].x, buf[1].y, buf[2].x, buf[2].y);
                //printf("%d\t%d\t%d\n", verticesIdx[0], verticesIdx[1], verticesIdx[2]);
                //imshow("Delaunay", imgShow);
                //waitKey();
            }

        }

        CV_NEXT_SEQ_ELEM( elem_size, reader );

    }

    //RemoveDuplicate(tri);
    char title[100];
    sprintf_s(title, 100, "Delaunay: %d Triangles", tri.size());//tri存储的为3个顶点为一个vec3i，故tri.size()表示三角形的个数。
    imshow(title, imgShow);
    waitKey();
}

void main(int argc, char* argv[])

{
    Mat imgL(600,600,CV_8UC3);
    /************************************************************************/
    /* Delaunay triangulation                                               */
    /************************************************************************/
    cout<<"doing triangulation..."<<endl;
    vector<Vec3i> tri;

    vector<Point2f> vec_points;
    for(int  i = 0; i < 60; i++ )
    {
        Point2f fp = cvPoint2D32f( (float)(rand()%(imgL.cols-10)),//使点约束在距离边框10像素之内。
                                   (float)(rand()%(imgL.rows-10)));
        vec_points.push_back(fp);
    }

    TriSubDiv(vec_points, imgL, tri);


}