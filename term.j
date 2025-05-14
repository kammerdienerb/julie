### DRAWING ###
rows     = 0
cols     = 0
elements = (list)

paint =
    fn ()
        @term:clear
        foreach &element elements
            ((&element 'paint-fn)) &element
        @term:flush

add-element    = (fn (&elem) (append elements &elem))
newest-element = (' (elements ((len elements) - 1)) )

text =
    fn (row col s ...)
        o =
            object
                'type     : 'rect
                'paint-fn : (' paint-text)
                'row      : row
                'col      : col
                's        : s
        foreach arg ... (o <- arg)
        add-element o

paint-text =
    fn (&text)
        row    = (&text 'row)
        col    = (&text 'col)
        tcolor = (select ('color in &text) (&text 'color) nil)
        foreach char (chars (&text 's))
            if (tcolor != nil)
                @term:set-cell-fg row col tcolor
            @term:set-cell-char row col char
            col += 1

rect =
    fn (row col height width color ...)
        o =
            object
                'type     : 'rect
                'paint-fn : (' paint-rect)
                'row      : row
                'col      : col
                'height   : height
                'width    : width
                'color    : color
        foreach arg ... (o <- arg)
        add-element o

paint-rect =
    fn (&rect)
        &r     = (&rect 'row)
        &c     = (&rect 'col)
        &h     = (&rect 'height)
        &w     = (&rect 'width)
        &color = (&rect 'color)

        repeat r &h
            r += &r
            repeat c &w
                c += &c
                @term:set-cell-bg r c &color
        if ('text in &rect)
            c = &c
            tcolor = (select ('text-color in &rect) (&rect 'text-color) nil)
            foreach char (chars (&rect 'text))
                if (tcolor != nil)
                    @term:set-cell-fg &r c tcolor
                @term:set-cell-char &r c char
                c += 1

in-element =
    fn (&element row col)
        select ((&element 'type) == 'rect)
            and
                row >= (&element 'row)
                row  < ((&element 'row) + (&element 'height))
                col >= (&element 'col)
                col  < ((&element 'col) + (&element 'width))
            0


### CONTENT ###

create-elements =
    fn ()
        elements := (list)

#         f = (fopen-rd "build.sh")
#         row = 1
#         color = 0xffff00
#         foreach line (fread-lines f)
#             text row 1 line
#                 'color : color
#             color += 10
#             row += 1
#         fclose f

#         rect 30 1 2 4 0xff0000
#             'text       : "red box"
#             'text-color : 0xffffff
#             'on-click   :
#                 fn (&rect row col)
#                     (&rect 'text-color) = (~ (&rect 'text-color))
#                     (&rect 'color) = (~ (&rect 'color))
#                     paint
#         rect 35 6 4 8 0x0000ff
#             'on-click :
#                 fn (&rect row col)
#                     (&rect 'color) = (~ (&rect 'color))
#                     paint
#         rect 40 12 1 16 0x006060
#             'text       : "frame"
#             'text-color : 0xffffff

        text rows 1 "press 'q' to quit"


### INPUT ###

parse-input =
    fn ()
        elements = (list)

        rect rows 1 1 1 0xffffff
        &bar = (newest-element)

        text rows 1 "Loading"
            'color : 0x0000ff

        paint

        f      = (fopen-rd "orig.txt")
        lines  = (fread-lines f)
        nlines = (len lines)

        stalls = 0

        # TODO: get rid of more julie_args
        # TODO: match

        i = 0
        foreach &line lines
#             if ((i % 10000) == 0)
#                 (&bar 'width) = (sint (((float i) / nlines) * cols))
#                 paint

            fields = (splits &line "\t")

            if   ((fields 0) == "eustall") (stalls += 1)
            elif ((fields 0) == "string")  nil

            i += 1

        fclose f

        @term:exit


key-actions =
    object
        "q" : (' (@term:exit) )

@on-key =
    fn (key)
        if (key in key-actions)
            (key-actions key)

@on-mouse =
    fn (type action button row col)
        if ((action == 'down) and (button == 'left))
            foreach &elem elements
                if (('on-click in &elem) and (in-element &elem row col))
                    (&elem 'on-click) &elem row col

@on-resize =
    fn (rows cols)
        rows := rows
        cols := cols
        parse-input
        create-elements
        paint
