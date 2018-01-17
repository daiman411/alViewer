# alImageViewer
A lightweight image viewer and image raw parser

Note:
1. Solution was created by Visual studio 2015, and used MFC to build this application
2. Application support MDI, which means it is able to open multi-document and display them simultaneously
3. It embedded WIC(Windows Imaging Component) as it's kernal, so it support various image format, like bmp, jpeg, gif and etc..
4. GDI/GDI+/DDraw/D2D were implemented to rendering engin, thus it can change three types of rendering method to evaluate efficiency
(Absolution D2D is the best one because it used hardware to accelerate performance and visual quality)
5. It also a image raw parser which currently support several kinds of raw format, like pure raw 8bit / 16bits, mipi 10/12 bits raw and some other special raw foramt. It will support more image and raw format in the future to make it more complete

