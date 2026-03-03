v = "captured"

l =
    lambda
        do
            &v = v
            &v = 789
            printf "v = %\n" &v

v = "not captured"

(l)

factorial =
    fn (n)
        select (n <= 2)
            n
            n * (factorial (n - 1))

println
    factorial 5

foo = (fn (&y) "result of foo")

bar = (fn (&x) (&x = 456) "result of bar")

foo = bar

call =
    fn (&f &arg) (&f &arg)

change =
    fn (&f &g)
        &f = &g

change foo bar
zip = 123
println
    call foo zip
println zip

append (list 1 2 3 4 5) 6

my-list = (list 1 2 3 4 5 6)
&e = (my-list 1)
&e = 123
insert my-list (list 456 789) 1
&x = (my-list 1)

o =
    object
        "a" : 1
        "b" : 2

o <- ("c" : 123)

(typeof o) == "object"

sorted (list 4 9 1 56 4 456 "abc" "0")

keys o
values o

foreach key o
    key = 123

(o "b") += 121

&f = (o "b")

&f = (list 1 2 3)

&g = (&f 2)

l = (list 1 2 3)
foreach &a l
    &a = (2 * &a)
    println &a

l = (list 1 (list 2 3))
&p = (l 1)
&p = 456
&q = (l 1)
&p = 789

println my-list

x = 123
&y = x
&z = &y
&y = 456
printf "x  = %\n" x
printf "&y = %\n" &y
printf "&z = %\n" &z

x = ((0x0 + -1) & 0x000000fffffff00)
println
    num-fmt "llx" x

println &y
x = (123 + 456)
println x
println &z

println
    typeof x

y = 0
println
    or
        not x
        not 1
        0

println
    apply (' (1 + 2))

println
    typeof +

if x
    printf "this is a value: %\n" l
else
    printf "was not true"

compare =
    ' (n < 10)

println
    ` compare

n = 1
while (apply compare)
    printf "ahhh\n"
    n += 1

c =
    fn ()
        println (backtrace)
b =
    fn () (c)
a =
    fn () (b)
(a)

define-class Class
    'list : (list)

    'add-to-list :
        fn (&self ...)
            foreach &arg ...
                append (&self 'list) &arg

    'print :
        fn (&self)
            println "==== Check out my object: ===="
            println (&self 'list)
            println "=============================="

instance = (new-instance Class)

instance @ ('add-to-list "a" "b" "c")
instance @ ('print)
