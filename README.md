**프로젝트명 : Just Move**  
Platform : Unreal 5.4 (C++)  
1인 언리얼 게임 개발 학부 졸업 프로젝트  
스토리 및 연출 중심의 FPS + 3D 플랫포머 게임  
기본 캐릭터 및 총기 언리얼 게임 1인칭 템플릿 사용
초반부 2D 게임 연출, UI 기능, 캐릭터 위치 및 레벨 전환, 아이템 습득 및 배치, 하이라이트 기능 등 BluePrint로 구현    

전체 요약 PDF (구글 드라이브) : https://drive.google.com/file/d/1vGOtaxH2TMwdd6VoForusCfHq7yJ58xi/view?usp=sharing   
전체 요약 PDF (네이버 박스) : https://naver.me/xktf7u4X  
게임 플레이 영상 : https://www.youtube.com/watch?v=LbjHGsm3aO8  
게임 공유 링크 (구글 드라이브) : https://drive.google.com/file/d/1KPQBFWGDR7kMGLgSMH6o_ZmvgED4h2ni/view?usp=sharing    
게임 공유 링크 (네이버 박스) : https://naver.me/x0XKQ60X  
자세한 내용 참고 : https://blob-thinking.tistory.com/849

**코드 참고** 
`GameAnimInstance.cpp / .h`  
 - 캐릭터 앉기 모션 애니메이션 할당
`GameCharacter.cpp / .h`  
 - 캐릭터 움직임 관련 기능 구현, 앉기 모션에 따른 카메라 이동 구현 
`GameDoor.cpp / .h` 
- 레이저를 통해 활성화되는 문 구현
`GamePlatform.cpp / .h` 
- 플랫폼 할당 및 플랫폼에 누적된 충격량 저장 및 부여
`GamePlayerController.cpp / .h` 
- 매핑 콘텍스트 할당
`GameProjectile.cpp / .h` 
- 발사체 할당 및 플랫폼에 닿을 시 충격량 부여 및 파괴 구현
`LaserBeam.cpp / .h` 
- 레이저 빔 이펙트 할당 및 활성화 (무기 2번 기능)
`TP_PickUpComponent.cpp / .h` 
- 무기 자동 습득 기능 (기본 언리얼 1인칭 게임 템플릿 그대로 사용)
`TP_WeaponComponent.cpp / .h` 
- 무기 관련 기능 및 누적 충격량 활성화 기능 구현 (기본 언리얼 1인칭 게임 템플릿 참고)

**맵 에셋**  
용량 문제로 push 불가하여 따로 기재
- Edith Finch: Classrooms and Bedrooms (언리얼 마켓 플레이스)
- Edith Finch: Molloy Room (언리얼 마켓 플레이스)
- Edith Finch: Edie Room (언리얼 마켓 플레이스)
- Edith Finch: House and Common Areas (언리얼 마켓 플레이스)
- Edith Finch: House Grounds (언리얼 마켓 플레이스)
- Edith Finch: Barbara Room (언리얼 마켓 플레이스)
- Edith Finch: Sam Room (언리얼 마켓 플레이스)
- Edith Finch: Beach Wedding (언리얼 마켓 플레이스)
- Edith Finch: Cannert and LowPoly Kingdom (언리얼 마켓 플레이스)
