screen =
    object
        "height" : 0
        "width"  : 0
        "pixels" : (list)

resize =
    fn (&screen new-height new-width)
        &h = (&screen "height")
        &w = (&screen "width")

        &h = new-height
        &w = new-width

        &p = (&screen "pixels")

        n = (&h * &w)

        while ((len &p) > n)
            pop &p
            nil

        while ((len &p) < n)
            append &p 0x303030
            nil

set-pixel =
    fn (&screen row col color)
        &w = (&screen "width")

        idx = ((&w * row) + col)
        ((&screen "pixels") idx) = color

rgb-to-term-fg =
    fn (&rgb)
        fmt "\e[38;2;%;%;%m"
            (&rgb & 0xff0000) >> 16
            (&rgb & 0x00ff00) >> 8
            &rgb & 0x0000ff
rgb-to-term-bg =
    fn (&rgb)
        fmt "\e[48;2;%;%;%m"
            (&rgb & 0xff0000) >> 16
            (&rgb & 0x00ff00) >> 8
            &rgb & 0x0000ff

display =
    fn (&screen)
        &h = (&screen "height")
        &w = (&screen "width")
        &p = (&screen "pixels")

        repeat r (&h / 2)
            r *= 2
            repeat c &w
                top-idx = ((r * &w) + c)
                bot-idx = (top-idx + &w)
                print
                    rgb-to-term-fg (&p top-idx)
                    rgb-to-term-bg (&p bot-idx)
                    "▀"
            print "\e[0m\n"

resize screen 64 64

set-pixel screen 1 1 0xff0000

display screen
