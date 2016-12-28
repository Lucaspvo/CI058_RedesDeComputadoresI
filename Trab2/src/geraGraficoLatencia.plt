set title 'Grafico Latencia'
set ylabel 'Latencia(s)'
set xlabel 'msg/s'
set yrange[0:100]
set autoscale
set terminal png
set output 'Grafico_Latencia.png'
plot 'Latencia.out' with lines
