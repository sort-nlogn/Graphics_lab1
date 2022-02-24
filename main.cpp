#include <graphics.h>
#include <iostream>
#include <thread>
#include <math.h>

#define scale_factor 0.08

using namespace std;

struct segment{
    double x1, y1, x2, y2, cx, cy;
    bool is_selected = false;
    segment(double x1, double y1, double x2, double y2){
        this->x1 = x1;   
        this->y1 = y1;
        this->x2 = x2;   
        this->y2 = y2; 
        this->cx = (x1 + x2) / 2;
        this->cy = (y1 + y2) / 2;
    }
};

void rotate_segment(segment &l, double theta){
    double c = cos(theta), s = sin(theta);
    double x1_tmp = c*(l.x1 - l.cx) - s*(l.y1 - l.cy) + l.cx;
    double y1_tmp = s*(l.x1 - l.cx) + c*(l.y1 - l.cy) + l.cy;

    l.x1 = x1_tmp; l.y1 = y1_tmp;

    double x2_tmp = c*(l.x2 - l.cx) - s*(l.y2 - l.cy) + l.cx;
    double y2_tmp = s*(l.x2 - l.cx) + c*(l.y2 - l.cy) + l.cy;

    l.x2 = x2_tmp; l.y2 = y2_tmp;
}

void move_segment(segment &s, double dx, double dy){
    s.x1 += dx, s.x2 += dx;
    s.y1 += dy, s.y2 += dy;
    s.cx += dx, s.cy += dy;
}

void scale_segment(segment &s, int dir){
    double v[2] = {(s.x1 - s.cx) * (1 - scale_factor * -dir),
                    (s.y1 - s.cy) * (1 - scale_factor * -dir)};
    s.x1 = v[0] + s.cx;
    s.y1 = v[1] + s.cy;
    
    s.x2 = -v[0] + s.cx;
    s.y2 = -v[1] + s.cy;

}

void handle_click(segment &s1, segment &s2, int x, int y){
    segment &s = s1.is_selected ? s2: s1;
    double a = -(s.y2 - s.y1);
    double b = (s.x2 - s.x1);
    double c = -(a*s.x1 + b*s.y1);
    double dist = abs(a*x + b*y + c) / sqrt(a*a + b*b);
    if(dist < 9.0){
        if(s1.is_selected){
            s2.is_selected = true;
            s1.is_selected = false;
        }else{
            s1.is_selected = true;
            s2.is_selected = false;
        }
    }
}

void keyboard_callback(int window, segment &s1, segment &s2){
    while(1){
        setcurrentwindow(window);
        char c = (char)getch();
        switch ((int)c){
        case 43:
            s1.is_selected? scale_segment(s1, 1): scale_segment(s2, 1);
            break;
        case 45:
            s1.is_selected? scale_segment(s1, -1): scale_segment(s2, -1);
            break;
        case 49:
            s1.is_selected? rotate_segment(s1, 0.256): rotate_segment(s2, 0.256); 
            break;
        case 50:
            s1.is_selected? rotate_segment(s1, -0.256): rotate_segment(s2, -0.256); 
            break;
        case 97:
            s1.is_selected? move_segment(s1, -10.0, 0.0): move_segment(s2, -10.0, 0.0);
            break;
        case 115:
            s1.is_selected? move_segment(s1, 0.0, 10.0): move_segment(s2, 0.0, 10.0);
            break;
        case 100:
            s1.is_selected? move_segment(s1, 10.0, 0.0): move_segment(s2, 10.0, 0.0);
            break;
        case 119:
            s1.is_selected? move_segment(s1, 0.0, -10.0): move_segment(s2, 0.0, -10.0);
            break;
        }
        this_thread::sleep_for(chrono::milliseconds(1));
    }
}

void bresenham_line(int x1, int y1, int x2, int y2){
    int steep = abs(y2 - y1) > abs(x2 - x1);

    if(steep){
        swap(x1, y1);
        swap(x2, y2);
    }

    if(x1 > x2){
        swap(x1, x2);
        swap(y1, y2);
    }

    int dx = x2 - x1;
    int dy = abs(y2 - y1);

    int y_step = dy == 0 ? 0: y1 < y2? 1: -1;

    int err = -dx; dx *= 2;
    int y = y1; dy *= 2;
    for(int x = x1; x <= x2; x++){
        int x_disp = steep? y: x;
        int y_disp = steep? x: y;
        if(x_disp < 620.0 && x_disp > 0.0 && y_disp < 480.0 && y_disp > 0.0){
            putpixel(x_disp, y_disp, RGB(255, 255, 0));
        }
        err += dy;
        if(err > 0){
            err -= dx;
            y += y_step;
        }
    }
}

int main(int argc, char const *argv[]){
    segment s1(100.0, 10.0, 100.0, 250.0);
    segment s2(450.0, 85.0, 450.0, 420.0);
    s1.is_selected = true;

    int win = initwindow(620, 480);

    thread th(keyboard_callback, win, ref(s1), ref(s2));
    th.detach();

    int mouse_x = -1, mouse_y = -1;
    while(1){
        getmouseclick(WM_LBUTTONDBLCLK, mouse_x, mouse_y);
        if(mouse_x != -1 && mouse_y != -1){
            handle_click(s1, s2, mouse_x, mouse_y);
        }
        setcolor(RGB(255, 255, 255));
        outtextxy(425, 30, (char*)"Selected: ");
        setcolor(s1.is_selected? RGB(255, 255, 0): RGB(0, 255, 0));
        outtextxy(500, 30, s1.is_selected? (char*)"A": (char*)"B");

        setcolor(RGB(255, 255, 0));
        bresenham_line((int)s1.x1, (int)s1.y1, (int)s1.x2, (int)s1.y2);

        setcolor(RGB(0, 255, 0));
        line((int)s2.x1, (int)s2.y1, (int)s2.x2, (int)s2.y2);

        swapbuffers();
        clearviewport();
        this_thread::sleep_for(chrono::milliseconds(1));

    }

    return 0;
}
