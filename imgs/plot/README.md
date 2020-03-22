# INTRODUCTION
This directory contains general-purpose plotting tools built upon the the GNUplot streaming interface library.

# USAGE
Within this build environment, all one needs to do to utilize these functions is to include this local library

    #include "imgs/plot/plot.h"


and be sure to include <span style="font-family:Courier">rit::plot</span> in the <span style="font-family:Courier">target\_link\_libraries</span> directive in your project's <span style="font-family:Courier">CMakeLists.txt</span> file.

##### From the command line (not recommended):
To compile code using the provided plotting functions based on the
gnuplot streaming library, use the following command on the CIS servers.

    # g++ -Isrc/cpp/rit/imgs/plot -I/usr/local/include/opencv4 -Isrc/cpp/rit -L/usr/lib64 -lboost_iostreams -lopencv_core -o <your binary name> <your source name>.cpp src/cpp/rit/imgs/plot/plot2d/Plot2d.cpp src/cpp/rit/imgs/plot/plot2d/Params.cpp

This command assumes the recommended directory structure has been followed, namely

    src
    └── cpp
       └── rit
           └── imgs
               ├── plot
               │   ├── plot2d
               │   │   ├── Params.cpp
               │   │   ├── Params.h
               │   │   ├── Plot2d.cpp
               │   │   └── Plot2d.h
               │   └── plot.h
               └── third_party
                   └── gnuplot-iostream.h

# EXAMPLES
Here is an example code illustrating how graph single and multiple plots using both <span style="font-family:Courier">std::vector</span> and <span style="font-family:Courier">cv::Mat</span> data types.

    #include <vector>

    #include <opencv2/core.hpp>

    #include "imgs/plot/plot.h"

    using namespace std;

    int main() {
      plot::plot2d::Params params;
      params.set_x_label("x");
      params.set_y_label("y");
      params.set_linestyle(params.LINES + params.POINTS);
    
      // Vector
      std::vector<double> x;
      std::vector<double> y;
      for (int i = 0; i <= 50; i++) {
        x.push_back((double)i);
        y.push_back((double)i * 50);
      }
      plot::Plot2d(x, y, params);
    
      // Vector of Vectors
      std::vector<double> x1;
      std::vector<double> y1;
      for (int i = 0; i <= 50; i++) {
       x1.push_back((double)i);
        y1.push_back((double)i * 50);
      }
      std::vector<double> x2;
      std::vector<double> y2;
      for (int i = 0; i <= 50; i=i+10) {
        x2.push_back((double)i);
        y2.push_back((double)i * (double)i);
      }
      std::vector< std::vector<double> > xs;
      xs.push_back(x1);
      xs.push_back(x2);
      std::vector< std::vector<double> > ys;
      ys.push_back(y1);
      ys.push_back(y2);
      plot::Plot2d(xs, ys, params);

      // Single vector in a cv::Mat_<double>
      cv::Mat_<double> xm = cv::Mat_<double>::zeros(1, 51);
      cv::Mat_<double> ym = cv::Mat_<double>::zeros(1, 51);
      for (int i = 0; i <= 50; i++) {
        xm(0, i) = i;
        ym(0, i) = i * 50;
      }
      plot::Plot2d(xm, ym, params);

      // Multiple vectors in a cv::Mat_<double>
      cv::Mat_<double> xms;
      cv::Mat_<double> yms;
      xms = cv::Mat_<double>::zeros(2, 51);
      yms = cv::Mat_<double>::zeros(2, 51);
      for (int i = 0; i <= 50; i++) {
        xms(0, i) = i;
        yms(0, i) = i * 50;
        xms(1, i) = i;
        yms(1, i) = i * i;
      }
      plot::Plot2d(xms, yms, params);

      params.Print();

      return EXIT_SUCCESS;

# FUNCTIONS
### For setting plot parameters

###### Constructors for the plot parameters object

    Params();

With plot labels

    Params(std::string x_label, std::string y_label);

###### Destructor for the plot parameters object

    ~Params();

###### Accessor and mutator for the x\_label member

    std::string x_label() const;
    void set_x_label(std::string x_label);

###### Accessor and mutator for the y\_label member

    std::string y_label() const;
    void set_y_label(std::string y_label);

###### Accessor and mutator for the title member

    std::string title() const;
    void set_title(std::string title);

###### Accessor and mutator for the x\_min and x\_max members

    double x_min() const;
    void set_x_min(double x_min);
    double x_max() const;
    void set_x_max(double x_max);

###### Accessor and mutator for the y\_min and y\_max members

    double y_min() const;
    void set_y_min(double y_min);
    double y_max() const;
    void set_y_max(double y_max);

###### Accessor and mutator for the x\_tics and y\_tics members

    bool x_tics() const;
    void set_x_tics(bool x_tics);
    bool y_tics() const;
    void set_y_tics(bool y_tics);

###### Accessor and mutator for the linestyle member

    int linestyle() const;
    void set_linestyle(int linestyle);

where linestyle can take on <span style="font-family:Courier">\<object\>.POINTS</span>, <span style="font-family:Courier">\<object\>.LINES</span>, or the sum <span style="font-family:Courier">\<object>.LINES + \<object\>.POINTS</span>.

###### Accessor and mutator for the xvline member

    double xvline() const;
    void set_xvline(double xvline);

###### Accessor and mutator for the destination filename member

    std::string destination_filename() const;
    void set_destination_filename(std::string dst_filename);

###### Print function to carry out a diagnostic dump of object values

    void Print() const;

### Convenience functions for plotting

    /** Convenience function for plotting two-dimensional vector data
     *
     *  \param[in] x independent variable vector of type T1
     *  \param[in] y dependent variable vector of type T2
     *  \param[in] p plot parameters object of type plot2d::Params
     */
    template <class T1, class T2>
    void Plot2d(const std::vector<T1>& x, const std::vector<T2>& y,
                const plot2d::Params& params);
<br/>

    /** Convenience function for plotting multiple two-dimensional vector data
     *
     *  \param[in] x independent variable vector of vectors of type T1
     *  \param[in] y dependent variable vector of vectors of type T2
     *  \param[in] p plot parameters object of type plot2d::Params
     */
    template <class T1, class T2>
    void Plot2d(const std::vector<std::vector<T1> >& x,
                const std::vector<std::vector<T2> >& y,
                const plot2d::Params& params);
<br/>

    /** Convenience function for plotting two-dimensional vector data stored
     *  in the row(s) of cv::Mat_
     *
     *  \param[in] x independent variable cv::Mat_ with 1 or multiple rows
     *  \param[in] y dependent variable cv::Mat_ with 1 or multiple rows
     *  \param[in] p plot parameters object of type plot2d::Params
     */
    template <class T1, class T2>
    void Plot2d(const cv::Mat_<T1> x, const cv::Mat_<T2> y,
                const plot2d::Params params);
<br/>

    /** Convenience function for plotting two-dimensional data described by the
     *  provided function over the interval [x_min, x_max] as defined in the
     *  provided parameters
     *
     *  \param[in] f
     *     function you would like to plot (prototype cannot be ambiguous)
     *  \param[in] params
     *     plot parameters object of type plot2d::Params
     *  \param[in] n
     *     the number of points at which to define the discrete representation of
     *     the function [default is 100]
     */
    template <class CALLABLE>
    void Plot2d(const CALLABLE f, plot::plot2d::Params params,
                size_t n = 100);

# REQUIREMENTS
* C++ compiler that supports C++17 dialect/ISO standard

# DEPENDENCIES
* Boost.Iostreams
* OpenCV Core

