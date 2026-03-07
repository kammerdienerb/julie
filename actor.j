do-join =
    fn ()
        (actor-join a)

a =
    actor-spawn
        '
            do
                while (do (msg = (actor-receive)) (msg != "stop"))
                    printf "actor: got %\n" msg
                error "ahhh"

repeat i 5
    actor-send a i
    sleep 0.5

actor-send a "stop"
(do-join)
