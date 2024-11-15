#!/bin/sh
mkdir act-out-cutfreq
#test con lake_large 100K SOA
../build/imtool-soa/imtool-soa ../in/lake-large.ppm act-out-cutfreq/lake-large-100K-soa.ppm cutfreq 100000

#Salida de mi funcion
lake_large_out_100K="act-out-cutfreq/lake-large-100K-soa.ppm"
#Salida de referencia
res_exp="exp-out-cutfreq/lake-large-100K.ppm"

if cmp "$lake_large_out_100K" "$res_exp"; then
        echo "Test con imagen a 100000 lake-large.ppm correcto"
else
        echo "Test fallido con imagen a 100000 lake-large.ppm"
        exit 1
fi

#test con lake_large 162K SOA
../build/imtool-soa/imtool-soa ../in/lake-large.ppm act-out-cutfreq/lake-large-162K-soa.ppm cutfreq 162000

#Salida de mi funcion
lake_large_out_162K="act-out-cutfreq/lake-large-162K-soa.ppm"
#Salida de referencia
res_exp="exp-out-cutfreq/lake-large-162K.ppm"

if cmp "$lake_large_out_162K" "$res_exp"; then
        echo "Test con imagen lake-large.ppm a 162000 correcto"
else
        echo "Test fallido con imagen a 162000 lake-large.ppm"
        exit 1
fi
