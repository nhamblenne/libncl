pass
pass label
next
next label
exit
exit label
return
return 42
a := 42
a, b := 36, 42
s(1), c(2) := sin x, cos y
proc
proc a
proc a, b
fn() a
fn(a) b
pass when true
return unless false
a := b when pred b
if cond then
   foo
end
if cond then
   foo
   bar
end
if cond then
   foo
else
   bar
   qux
end
if cond then
   foo; bar
elsif cond then
   foo
elsif cond then
   bar
else
   qux
end
begin
   foo
   bar
end
loop
   a
   exit when b
   c
end
while cond loop
   print c
   d := e + d
end
label: loop
   exit label
end
for id in exp loop
   print id
end
func := { x => x + 2 }
proc := { x, y do
   foo
   bar
}
