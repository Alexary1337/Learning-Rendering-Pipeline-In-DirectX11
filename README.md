# My simple project
My simple project where I am getting started with rendering pipeline using DirectX SDK. Currently my knowledge in this is not so good so project is expanding only with tutorials help.
This project is based on [Rastertek's tutorials](http://www.rastertek.com).

## Dependencies ##
* [Assimp v3.3.1](http://assimp.org/) [(Source)](https://github.com/assimp/assimp)

### Current progress showcase:
![Promo](ReadmeData/skull.png)
![Promo](ReadmeData/skullWF.png)
![Promo](ReadmeData/pr1.png)
![Promo](ReadmeData/pr2.png)

## HM1
### 1.
func number <br/>
&nbsp; &nbsp; &nbsp; | number == 0 = [] <br/>
&nbsp; &nbsp; &nbsp; | otherwise = mod number 10 : func (div number 10) <br/>
### 2.
func word <br/>
&nbsp; &nbsp; &nbsp; | length word <= 1 = True <br/>
&nbsp; &nbsp; &nbsp; | head word /= last word = False <br/>
&nbsp; &nbsp; &nbsp; | otherwise = func (init(tail word)) <br/>
### 3.
max [x] = x <br/>
max (x:xs) <br/> 
&nbsp; &nbsp; &nbsp; | x > maxTail = x <br/>
&nbsp; &nbsp; &nbsp; | otherwise = maxTail <br/>
&nbsp; &nbsp; where maxTail = max xs <br/>
### 4.
func num <br/>
&nbsp; &nbsp; &nbsp; | num == 2 = True <br/>
&nbsp; &nbsp; &nbsp; | mod num 2 == 0 = func(div num 2) <br/>
&nbsp; &nbsp; &nbsp; | otherwise = False <br/>

## HM2
### 1. ZipWith
zipWith _ [] _ = [] <br/>
zipWith _ _ [] = [] <br/>
zipWith f (a:as) (b:bs) = [f a b] ++ zipWith f as bs <br/>
sum a b = a + b <br/>
### 2. Flip
flip f a b = f a b <br/>
sum a b = a + b <br/>
### 3. Map
map _ [] = [] <br/>
map f (a:as) = [f a] ++ map f as <br/>
sum a b = a + b <br/>
### 4. Filter
filter _ [] = [] <br/>
filter f (a:as) <br/> 
&nbsp; &nbsp; &nbsp; | f a == True = [a] ++ filter f as <br/>
&nbsp; &nbsp; &nbsp; | otherwise = filter f as <br/>
isOdd a <br/>
&nbsp; &nbsp; &nbsp; | rem a 2 /= 0 = True <br/>
&nbsp; &nbsp; &nbsp; | otherwise = False <br/>
