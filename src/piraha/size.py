fac = 1+1/4.+1/16.
diff = 1+fac - fac**3
print diff
init = 16
nextv = init + (init>>2) + (init>>4)
print nextv
print 1.0*nextv/init
