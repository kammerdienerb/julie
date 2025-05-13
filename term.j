@on-key =
    fn (key)
        println key
        @term:clear
@on-mouse =
    fn (type action button row col)
        printf "% % % % %\n" type action button row col
@on-resize =
    fn (rows cols)
        println rows cols

@term:set-cell-bg 1 1 0xff0000
@term:flush

# term:creset = "\e[0m"
# term:rgb-fg =
#     fn (&rgb)
#         fmt "\e[38;2;%;%;%m" ((&rgb & 0xff0000) >> 16) ((&rgb & 0x00ff00) >> 8) (&rgb & 0x0000ff)
# term:rgb-bg =
#     fn (&rgb)
#         fmt "\e[48;2;%;%;%m" ((&rgb & 0xff0000) >> 16) ((&rgb & 0x00ff00) >> 8) (&rgb & 0x0000ff)

# screen =
#     object
#         'height   : 0
#         'width    : 0
#         'pixels   : (list)
#         'elements : (list)

# resize =
#     fn (&screen new-height new-width)
#         (&screen 'height) = new-height
#         (&screen 'width)  = new-width

# set-cell =
#     fn (&screen row col fg bg text)
#         if (row > 0) and (col > 0)
#             row -= 1
#             col -= 1
#             &w = (&screen 'width)

#             idx = ((&w * row) + col)
#             ((&screen 'pixels) idx) =
#                 object
#                     'fg   : fg
#                     'bg   : bg
#                     'text : text

# display =
#     fn (&screen)
#         &h = (&screen 'height)
#         &w = (&screen 'width)
#         &p = (&screen 'pixels)

#         repeat r &h
#             repeat c &w
#                 idx = ((r * &w) + c)
#                 print
#                     term:rgb-fg ((&p idx) 'fg)
#                     term:rgb-bg ((&p idx) 'bg)
#                     " "
#             println term:creset

# paint-rect =
#     fn (&screen &rect)
#         &r     = (&rect   'row)
#         &c     = (&rect   'col)
#         &h     = (&rect   'height)
#         &w     = (&rect   'width)
#         &color = (&rect   'color)
#         &sh    = (&screen 'height)
#         &sw    = (&screen 'width)

#         repeat r &h
#             r += &r
#             if (r < &sh)
#                 repeat c &w
#                     c += &c
#                     if (c < &sw)
#                         set-cell &screen r c 0 &color " "

# paint =
#     fn (&screen)
#         &h = (&screen 'height)
#         &w = (&screen 'width)
#         &p = (&screen 'pixels)

#         n = (&h * &w)

#         &p = (list)

#         blank =
#             object
#                 'fg   : 0
#                 'bg   : 0x303030
#                 'text : " "

#         while ((len &p) < n)
#             append &p blank

#         foreach element (&screen 'elements)
#             ((element 'paint-fn)) &screen element

# rect =
#     fn (row col height width color ...)
#         o =
#             object
#                 'type     : 'rect
#                 'paint-fn : (' paint-rect)
#                 'row      : row
#                 'col      : col
#                 'height   : height
#                 'width    : width
#                 'color    : color
#         foreach arg ... (o <- arg)
#         append (screen 'elements) o

# in-element =
#     fn (&element row col)
#         select ((&element 'type) == 'rect)
#             and
#                 row >= (&element 'row)
#                 row  < ((&element 'row) + (&element 'height))
#                 col >= (&element 'col)
#                 col  < ((&element 'col) + (&element 'width))
#             0

# resize screen 32 64

# update = 0


# (screen 'elements) = (list)

# rect 1 1 2 4 0xff0000
#     'on-click :
#         fn (&r row col)
#             (&r 'color) = 0x00ff00
#             update := 1

# rect 6 6 4 8 0x0000ff
#     'on-click :
#         fn (&r row col)
#             (&r 'color) = 0x00ffff
#             update := 1

# draw =
#     fn()
#         paint   screen
#         display screen

# draw

# continue = 1
# while continue
#     event = (@term:get-event)

#     if (event != nil)
#         if ((event 'type) == 'key)
#             if ((event 'key) == "q") (continue = 0)

#         elif ((event 'type) == 'mouse)
#             if (((event 'action) == 'down) and ((event 'button) == 'left))
#                 row = (event 'row)
#                 col = (event 'col)
#                 foreach &element (screen 'elements)
#                     if (('on-click in &element) and (in-element &element row col))
#                         (&element 'on-click) &element row col

#     if update
#         draw
#         update = 0
