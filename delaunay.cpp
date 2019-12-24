//
// Created by tr on 19-12-20.
//

#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

typedef struct _TRIANGLE_DESC_
{
    Point pt1, pt2, pt3;
    _TRIANGLE_DESC_(const Point _pt1, const Point _pt2, const Point _pt3):
            pt1(_pt1), pt2(_pt2), pt3(_pt3){}//括号内没有包含任何东西 各成员置0

}TRIANGLE_DESC;//这里直接就创建了结构变量


vector<TRIANGLE_DESC> delaunayAlgorithm(const Rect boundRc,const vector<Point>& points)
{
    if (points.empty())
    {
        return vector<TRIANGLE_DESC>();
    }
    vector<TRIANGLE_DESC> result;

    vector<Vec6f> _temp_result;

    Subdiv2D subdiv2d(boundRc);//调用2维的平面分割 创建了一个subdiv2d的类的对象 object

    for (const auto point : points)
    {
        subdiv2d.insert(Point2f((float)point.x, (float)point.y));//调用类的方法
    }
    subdiv2d.getTriangleList(_temp_result);

    for (const auto _tmp_vec : _temp_result)
    {
        Point pt1((int)_tmp_vec[0], (int)_tmp_vec[1]);
        Point pt2((int)_tmp_vec[2], (int)_tmp_vec[3]);
        Point pt3((int)_tmp_vec[4], (int)_tmp_vec[5]);
        result.push_back(TRIANGLE_DESC(pt1, pt2, pt3));
    }

    return result;

}
void showpoints(vector<TRIANGLE_DESC> result){
    //vector<TRIANGLE_DESC> result;
    //Point pt1, pt2, pt3;
    for (int i=0;i<result.size();i++) {
        cout << "the tri points are: " << endl;
        cout <<"{"<< result[i].pt1 << ", " << result[i].pt2 <<", "<<result[i].pt3<< "}"<<endl;
        //cout<<result[i+1].pt1<<" "<<result[i+1].pt2<<endl;
        //cout<<result[i+2].pt1<<" "<<result[i+2].pt2<<"}"<<endl;
    }
}

int main(int argc, char* argv[])
{
    const int width = 400;
    const int height = 400;
    Mat srcImg(height, width, CV_8UC3, Scalar(255,255,255));
    const vector<Point> testPoints = {
            Point(23, 45), Point(243, 145), Point(308, 25),
            Point(180, 230), Point(343, 145), Point(108, 25)
    };
    for (const auto point : testPoints)
    {
        circle(srcImg, point, 1, Scalar(0), 2);
    }

    /**
     * opencv Rect 类
     * Rect_(_Tp _x, _Tp _y, _Tp _width, _Tp _height)，定义一个左上角点坐标为(_x, _y)的_width*_height矩形窗口；
     */
    //const Rect pageRc(0, 0, width, height);//waht is this?
    const Rect pageRc(0, 0, width-50, height-50);

    const auto triangles = delaunayAlgorithm(pageRc,testPoints);

cout<<triangles.size()<<endl;
showpoints(triangles);
    for (const auto triangle : triangles)
    {

        line(srcImg, triangle.pt1, triangle.pt2, Scalar(255, 0, 0));
        line(srcImg, triangle.pt1, triangle.pt3, Scalar(255, 0, 0));
        line(srcImg, triangle.pt2, triangle.pt3, Scalar(255, 0, 0));
    }
    imshow("src", srcImg);//为什么会有超出图像之外的点呢？
    imwrite("/home/tr/Documents/delaunay/delaunay tri.jpg",srcImg);
    waitKey(0);

    return 0;
}