# Quickstart: 034-full-features

## 빌드

```bash
make re        # 전체 빌드
make bonus     # 보너스 빌드 (= make all)
norminette src/ includes/
```

## 테스트 씬 파일

### 1. Camera Yaw 테스트
```
# scenes/test_yaw.rt
A  0.2  255,255,255
C  0,5,0  0,0,1  70
L  -40,50,0  0.7  255,255,255

sp  0,0,20  10  255,0,0
sp  20,0,20  10  0,255,0
sp  -20,0,20  10  0,0,255
pl  0,0,0  0,1,0  200,200,200
```
검증: 키 1/3으로 좌우 회전 → 구들이 시야에서 이동

### 2. Checkerboard 테스트
```
# scenes/test_checker.rt
A  0.2  255,255,255
C  0,10,-20  0,-0.3,1  70
L  0,30,0  0.8  255,255,255

pl  0,0,0  0,1,0  255,255,255  checker:0,0,0
sp  0,3,10  6  255,0,0  checker:255,255,0
cy  10,3,10  0,1,0  4  8  0,0,255  checker:255,255,255
co  -10,5,10  0,1,0  6  8  0,200,0  checker:200,200,0
```
검증: 바닥에 흑백 체커보드, 구에 적황 체커, 원기둥에 청백 체커, 원뿔에 녹황 체커

### 3. Multi-Light 테스트
```
# scenes/test_multilight.rt
A  0.1  255,255,255
C  0,5,-15  0,0,1  70
L  -15,15,0  0.6  255,100,100
L  15,15,0  0.6  100,100,255
L  0,15,-10  0.4  100,255,100

sp  0,3,10  8  200,200,200
pl  0,0,0  0,1,0  150,150,150
```
검증: 3색 조명이 구에 혼합, 각 광원별 독립 그림자, `=`키로 전환

### 4. Cone 테스트
```
# scenes/test_cone.rt
A  0.2  255,255,255
C  0,10,-20  0,-0.2,1  70
L  -20,30,0  0.7  255,255,255

co  0,5,10  0,1,0  8  10  255,128,0
pl  0,0,0  0,1,0  200,200,200
sp  -10,3,10  6  100,100,255
```
검증: 원뿔 형태 확인, 이동/회전/리사이즈 동작

### 5. Bump Map 테스트
```
# scenes/test_bump.rt
A  0.2  255,255,255
C  0,5,-15  0,0,1  70
L  -20,20,0  0.7  255,255,255

sp  0,5,10  8  200,150,100  bump:textures/brick.xpm
pl  0,0,0  0,1,0  200,200,200
```
검증: 구 표면에 울퉁불퉁한 질감 효과

### 6. Checker + Bump Map 복합 테스트
```
# scenes/test_combined.rt
A  0.2  255,255,255
C  0,8,-15  0,-0.2,1  70
L  -20,20,0  0.7  255,255,255

sp  0,5,10  8  200,150,100  checker:50,50,50  bump:textures/brick.xpm
pl  0,0,0  0,1,0  255,255,255  checker:0,0,0
cy  10,4,10  0,1,0  4  8  0,100,255  checker:255,255,255  bump:textures/brick.xpm
```
검증: 구에 체커보드+범프 동시 적용, 평면에 체커보드만, 원기둥에 체커+범프 동시 적용

### 7. 에러 케이스 테스트
```bash
# 잘못된 원뿔 파라미터
echo 'A 0.2 255,255,255\nC 0,0,0 0,0,1 70\nco 0,0,10 0,0,1 -5 10 255,0,0' > /tmp/bad_cone.rt
./miniRT /tmp/bad_cone.rt
# Expected: Error\n + explicit message

# 존재하지 않는 범프맵
echo 'A 0.2 255,255,255\nC 0,0,0 0,0,1 70\nsp 0,0,10 10 255,0,0 bump:nonexistent.xpm' > /tmp/bad_bump.rt
./miniRT /tmp/bad_bump.rt
# Expected: Error\n + explicit message

# 미인식 보너스 토큰 (FR-009b)
echo 'A 0.2 255,255,255\nC 0,0,0 0,0,1 70\nsp 0,0,10 10 255,0,0 foo:bar' > /tmp/bad_option.rt
./miniRT /tmp/bad_option.rt
# Expected: Error\n + explicit message
```

## 수동 검증 체크리스트

- [ ] Camera yaw: 키 1(좌), 3(우) → 시야 회전
- [ ] Camera reset: S키 → yaw 포함 초기화
- [ ] Checkerboard: 평면/구/원기둥/원뿔에 바둑판 패턴
- [ ] Multi-light: 2+ 광원 혼합, `=`키 전환, 독립 그림자
- [ ] Cone: 올바른 형태, 이동/회전/리사이즈
- [ ] Bump map: 구 표면 요철 효과
- [ ] Combined: 체커보드+범프맵 동시 적용 (구/원기둥)
- [ ] Error: 잘못된 cone/bump → "Error\n" + 메시지
- [ ] `make bonus` 빌드 성공
- [ ] norminette 에러 0건
