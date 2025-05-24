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

get-color =
    fn (r)
        h = 0.0
        s = 0.5
        v = (0.75 + (((float ((r % 1000) + 1)) / 1000.0) * 0.15))

        R = 0.0
        G = 0.0
        B = 0.0
        C = (v * s)
        X = (C * (1 - (abs (((h / (3.14159 / 3.0)) % 2.0) - 1))))
        m = (v - C)

        if ((h >= 0.0) and (h < (3.14159 / 3.0)))
            R = C
            G = X
            B = 0
        elif ((h >= (3.14159 / 3.0)) and (h < ((2.0 * 3.14159) / 3.0)))
            R = X
            G = C
            B = 0
        elif ((h >= ((2.0 * 3.14159) / 3.0)) and (h < 3.14159))
            R = 0
            G = C
            B = X
        elif ((h >= (3.14159 / 2.0)) and (h < ((4.0 * 3.14159) / 3.0)))
            R = 0
            G = X
            B = C
        elif ((h >= ((4.0 * 3.14159) / 3.0)) and (h < ((5.0 * 3.14159) / 3.0)))
            R = X
            G = 0
            B = C
        elif ((h >= ((5.0 * 3.14159) / 3.0)) and (h < (2.0 * 3.14159)))
            R = C
            G = 0
            B = X

        (((sint ((R + m) * 255)) & 255) << 16) |
            (((sint ((G + m) * 255)) & 255) << 8) |
                (sint ((B + m) * 255)) & 255

draw-flame =
    fn (&frame row start-col width)
        if ((width >= 1) and (row >= 1))
            text = (&frame 'label)
            if (width == 1)
                text = ""
            else
                if (((len text) > (width - 1)) and (width > 2))
                    text =
                        fmt "%.." (substr text 0 (width - 3))
                text = (substr text 0 (width - 1))

            rect row start-col 1 width (get-color (&frame 'rand))
                'text : text

            sorted-children-labels = (list)
            foreach label (&frame 'children)
                &child = ((&frame 'children) label)
                append sorted-children-labels (label : (&child 'count))
                unref &child
            sorted-children-labels = (sorted sorted-children-labels (fn (a b) ((a 1) > (b 1))))

            child-offset = 0
            foreach &pair sorted-children-labels
                &child = ((&frame 'children) (&pair 0))

                child-width = (sint (((float (&child 'count)) / (float (&frame 'count))) * width))
                if (child-width < 1) (child-width = 1)

                if ((child-offset + child-width) >= width)
                    child-width = (width - child-offset)

                if (child-width > 0)
                    draw-flame &child (row - 1) (start-col + child-offset) child-width

                child-offset += child-width
                unref &child

create-elements =
    fn ()
        elements := (list)

        if (flame-graph != nil)
            draw-flame flame-graph rows 1 cols

        text 1 1 "press 'q' to quit"


### INPUT ###

flame-graph = nil

new-frame =
    fn (&label)
        object
            'label    : &label
            'rand     : (rand)
            'count    : 0
            'children : (object)

add-flame =
    fn (&frame &stack &count)
        if (len &stack)
            &children = (&frame 'children)

            fname = (&stack 0)
            erase &stack 0

            if (not (fname in &children))
                &children <- (fname : (new-frame fname))

            add-flame (&children fname) &stack &count

        (&frame 'count) += &count

parse-input =
    fn ()
        flame-graph := (new-frame "all")

        f = (fopen-rd "term.flamegraph")

        foreach &line (fread-lines f)
            stack = (split &line ";")
            s     = (split (pop stack) " ")
            count = (parse-int (s 1))
            append stack (s 0)

            add-flame flame-graph stack count

        fclose f

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

redraw =
    fn (rows cols)
        rows := rows
        cols := cols
        create-elements
        paint


@on-init =
    fn (rows cols)
        redraw rows cols
        parse-input
        redraw rows cols

@on-resize =
    fn (rows cols)
        redraw rows cols
