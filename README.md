# Learning Rendering Pipeline In DirectX11
My simple project where I am getting started with rendering pipeline using DirectX SDK. Currently my knowledge in this is not so good so project is expanding only with tutorials help.
This project is based on [Rastertek's tutorials](http://www.rastertek.com).
### Current progress showcase:
![Promo](ReadmeData/skull.png)
![Promo](ReadmeData/skullWF.png)


### Temporary

1.
func number
     | number == 0 = []
     | otherwise = mod number 10 : func (div number 10)

func word
     | length word <= 1 = True
     | head word /= last word = False
     | otherwise = func (init(tail word))

3.
max [x] = x  
max (x:xs)   
     | x > maxTail = x
     | otherwise = maxTail
   where maxTail = max xs  

4.
func num
       | num == 2 = True
       | mod num 2 == 0 = func(div num 2)
       | otherwise = False
