# === Actor model + error value examples ===

# Actors inherit the parent's global scope.
double =
    fn (x)
        x * 2

# --- Basic spawn + join ---
println (actor-join (actor-spawn (' (double 21) )))          # => 42
println (actor-join (actor-spawn (parse-julie "double 21"))) # => 42

# --- Storing code in a variable ---
my-code = (' (double 100) )
println (actor-join (actor-spawn my-code))           # => 200

# --- Messaging ---
@a =
    actor-spawn
        '
            do
                actor-send @parent "hello"
                actor-receive

println (actor-receive)                        # => "hello"
actor-send @a "world"
println (actor-join @a)                         # => "world"

# --- Actor failure => error value ---
@a = (actor-spawn (' (bad-symbol) ))
result = (actor-join @a)
println (typeof result)                  # => "error"

# --- Send to dead actor => error value ---
@a = (actor-spawn (' (42) ))
actor-join @a
println (typeof (actor-send @a "late"))         # => "error"

code = (' (1 + 2) )
println (code)                           # => 3

code = (parse-julie "double 5")
println (code)                           # => 10

# --- eval-sandboxed accepts list or string ---
println (eval-sandboxed (' (1 + 2) ))    # => 3

println (typeof (eval-sandboxed (' (bad) ))) # => "error"

# --- eval-sandboxed with bindings ---
println
    eval-sandboxed (' (x + y) )
        object
            'bindings :
                object
                    "x" : 10
                    "y" : 20             # => 30

# --- Spawn with config: blacklist + bindings ---
secret = "hidden"
@a =
    actor-spawn (' (secret) )
        object
            'blacklist : (list "secret")
println (typeof (actor-join @a))                # => "error"

@a =
    actor-spawn (' (extra) )
        object
            'bindings :
                object
                    "extra" : 99
println (actor-join @a)                         # => 99

# --- Shared functions ---
greet =
    fn (name)
        fmt "Hello, %!" name
println (actor-join (actor-spawn (' (greet "world") )))  # => "Hello, world!"

# --- Counter actor ---
@a =
    actor-spawn
        '
            do
                x = 0
                while (do (msg = (actor-receive)) (msg != "stop"))
                    match msg
                        "get"
                            actor-send @parent x
                        "inc"
                            x += 1
                x

actor-send @a "inc"
actor-send @a "inc"
actor-send @a "get"
println (actor-receive)                        # => 2
actor-send @a "stop"
println (actor-join @a)                         # => 2
