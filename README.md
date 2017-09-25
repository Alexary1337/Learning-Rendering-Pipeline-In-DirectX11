# Learning Rendering Pipeline In DirectX11
My simple project where I am getting started with rendering pipeline using DirectX SDK. Currently my knowledge in this is not so good so project is expanding only with tutorials help.
This project is based on [Rastertek's tutorials](http://www.rastertek.com).
### Current progress showcase:
![Promo](ReadmeData/skull.png)
![Promo](ReadmeData/skullWF.png)


### Temporary
<br/>
## 1.
<br/>
func number <br/>
&nbsp; &nbsp; &nbsp; | number == 0 = [] <br/>
&nbsp; &nbsp; &nbsp; | otherwise = mod number 10 : func (div number 10) <br/>
<br/>
## 2. 
<br/>
func word <br/>
&nbsp; &nbsp; &nbsp; | length word <= 1 = True <br/>
&nbsp; &nbsp; &nbsp; | head word /= last word = False <br/>
&nbsp; &nbsp; &nbsp; | otherwise = func (init(tail word)) <br/>
<br/>
## 3. 
<br/>
max [x] = x <br/>  
max (x:xs) <br/>   
&nbsp; &nbsp; &nbsp; | x > maxTail = x <br/>
&nbsp; &nbsp; &nbsp; | otherwise = maxTail <br/>
&nbsp; &nbsp; where maxTail = max xs <br/>
<br/>
## 4. 
<br/>
func num <br/>
&nbsp; &nbsp; &nbsp; | num == 2 = True <br/>
&nbsp; &nbsp; &nbsp; | mod num 2 == 0 = func(div num 2) <br/>
&nbsp; &nbsp; &nbsp; | otherwise = False <br/>
