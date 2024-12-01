# wsl-host-updater

WSL의 IP 주소를 자동으로 Windows hosts 파일에 동기화하는 Windows 서비스입니다.

## 기능
- WSL의 IP 주소를 자동으로 감지
- Windows hosts 파일에 `WSL_IP` 호스트 이름으로 자동 등록
- Windows 서비스로 동작하여 백그라운드에서 자동 실행
- Windows 시작 시 자동 실행

## 설치 방법

1. 관리자 권한으로 명령 프롬프트(CMD) 실행
2. 다음 명령어로 서비스 설치:
```cmd
wsl-host-updater.exe --install
```

## 제거 방법

1. 관리자 권한으로 명령 프롬프트(CMD) 실행
2. 다음 명령어로 서비스 제거:
```cmd
wsl-host-updater.exe --uninstall
```

## hosts 파일 확인 방법

서비스가 정상적으로 동작하면 `C:\Windows\System32\drivers\etc\hosts` 파일에 다음과 같은 내용이 추가됩니다:
```
172.x.x.x WSL_IP # wsl-ip-service
```

## 요구사항
- Windows 10 이상
- WSL2 설치
- 관리자 권한 (서비스 설치/제거 시)

## 주의사항
- hosts 파일을 직접 수정하므로 관리자 권한이 필요합니다
- 서비스 설치/제거 시 반드시 관리자 권한으로 실행해야 합니다
- WSL이 실행 중이지 않은 경우 IP 주소를 가져올 수 없습니다

## 문제해결

1. "서비스 설치 실패" 오류
   - 관리자 권한으로 실행했는지 확인
   - 기존 서비스가 이미 설치되어 있는지 확인

2. "WSL IP를 가져올 수 없음" 오류
   - WSL이 설치되어 있는지 확인
   - WSL이 실행 중인지 확인

## 기여 방법
이슈나 풀 리퀘스트는 언제나 환영합니다!