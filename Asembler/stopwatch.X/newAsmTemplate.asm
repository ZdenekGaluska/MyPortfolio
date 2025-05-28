.org 0x1000           ; Adresa za?�tku programu
.include "printlib.inc"  ; Vlo?en� knihovny pro pr�ci s displejem

.def setiny = r18        ; Registr pro setiny sekundy (0-99)
.def sekundy = r19       ; Registr pro sekundy (0-59)
.def minuty = r20        ; Registr pro minuty (0-99)
.def bezi = r21          ; P?�znak zda stopky b??� (0=stoj�, 1=b??�)
.def pomocny = r16       ; Pomocn� registr pro r?zn� operace
.def pozice = r17        ; Pozice na displeji
.def tlacitko = r22      ; Pro ulo?en� hodnoty stisknut�ho tla?�tka
.def old_tlacitko = r23  ; Pro detekci zm?ny stavu tla?�tka    
.def mezicas = r24       ; 0 = norm�ln� zobrazen�, 1 = zobrazen� mezi?asu
    
; Konstanty pro k�dy tla?�tek
.equ BTN_NONE = 0x0F     ; ?�dn� tla?�tko
.equ BTN_RIGHT = 0x00    ; (0b00000000)
.equ BTN_UP = 0x01       ; (0b00000001) - pou?ijeme jako RESET
.equ BTN_DOWN = 0x03     ; (0b00000010) - pou?ijeme jako STOP
.equ BTN_LEFT = 0x06     ; (0b00000011) - pou?ijeme jako START
.equ BTN_SELECT = 0x09   ; (0b00000100) - pou?ijeme jako MEZI?AS

; Zacatek programu - po resetu
.org 0
jmp start

; Zacatek programu - hlavni program
.org 0x100
start:
    
    ; volani funkci pro inicializaci knihoven
    call init_button 
    call init_disp
    call lcd_light_on
    
    ; nastaveni pocatecnich hodnot
    clr setiny
    clr sekundy
    clr minuty
    clr mezicas           ; Inicializace p?�znaku mezi?asu
    ldi bezi, 1
    
;==========================HLAVNI SMYCKA===================================
;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
hlavni_smycka:
    ; 1) Funkce pouziti tlacitek
    call zkontroluj_tlacitka
    
    ; 2) Kontrola jestli bezi cas, jinak se vrati na zacatek hlavni_smycky 
    cpi bezi, 1
    brne hlavni_smycka  ; Pokud neb??�, sko? zp?t na za?�tek smy?ky
    
    ; 3) Pocita tak aby to spozdilo program o jednu setinu
    call cekej_setinu
    
    ; 4) zvysi cas o jednu setinu k casu
    call zvys_cas
    
    ; 5) zobrazi cas na displej
    cpi mezicas, 1        ; Je aktvivni mezicas?
    breq skip_display     ; pokud bylo tak to preskoci na skip_display
    call zobraz_cas       ; zobrazi cely cas
    
    skip_display:
    ; 6) Skok zp?t na za?�tek hlavni smy?ky
    rjmp hlavni_smycka
    
;///////////////////////////////////////////////////////////////////////////
;========================KONEC HLAVNI SMYCKY===============================


;=====================Inicializace AD p?evodn�ku pro tla?�tka======================
; Tato funkce se vol� na za?�tku programu a nastavuje AD p?evodn�k
init_button:
    push pomocny
    
    ldi pomocny, (1<<ADEN) | (0b110<<ADPS0)
    sts ADCSRA, pomocny
    
    ldi pomocny, (0b01<<REFS0) | (1<<ADLAR)
    sts ADMUX, pomocny
    
    pop pomocny
    ret

;====================Funkce pro kontrolu tla?�tek==========================
zkontroluj_tlacitka:
    push pomocny
    ; ?�ST 1: Zah�jen� p?evodu z analogov�ho na digit�ln� hodnotu
    lds pomocny, ADCSRA
    ori pomocny, (1<<ADSC)
    sts ADCSRA, pomocny
    ; ?�ST 2: ?ek�n� na dokon?en� p?evodu
wait_conversion:
    lds pomocny, ADCSRA
    sbrc pomocny, ADSC
    rjmp wait_conversion
    ; ?�ST 3: ?ten� v�sledku a identifikace tla?�tka
    lds pomocny, ADCH
    ; Posun o 4 bity doprava
    lsr pomocny
    lsr pomocny
    lsr pomocny
    lsr pomocny
    ; Ulo?en� k�du tla?�tka
    mov tlacitko, pomocny
    ; ?�ST 4: Detekce zm?ny stavu tla?�tka
    cp tlacitko, old_tlacitko
    breq no_button_change
    ; Tla?�tko se zm?nilo, ulo?�me nov� stav
    mov old_tlacitko, tlacitko
    
    ;----------------Logika tla?�tek - STOP (DOWN)---------------------
    cpi tlacitko, BTN_DOWN
    brne not_stop
    ; Tla?�tko DOWN bylo stisknuto - zastav�me stopky
    ldi bezi, 0
    rjmp button_done
not_stop:
    ; ----------------Logika tla?�tek - RESET (UP)----------------------
    cpi tlacitko, BTN_UP
    brne not_reset
    ; Tla?�tko UP bylo stisknuto - resetujeme stopky
    ldi bezi, 0
    clr setiny
    clr sekundy
    clr minuty
    clr mezicas         ; Zru?�me i p?�padn� mezi?as
    call zobraz_cas     ; Aktualizujeme displej
    rjmp button_done
not_reset:
    ;--------------Logika tla?�tek - START (LEFT)------------------------
    cpi tlacitko, BTN_LEFT
    brne not_start
    ; Tla?�tko LEFT bylo stisknuto - spust�me stopky
    ldi bezi, 1
    rjmp button_done
not_start:
    ;----------------Logika tla?�tek - MEZI?AS (SELECT)-----------------------
    cpi tlacitko, BTN_SELECT
    brne button_done
    ; Tla?�tko SELECT bylo stisknuto - p?epneme zobrazen� mezi?asu
    ldi pomocny, 1
    eor mezicas, pomocny        ; P?epneme bit mezi?asu (0->1 nebo 1->0)
button_done:
no_button_change:
    pop pomocny
    ret
;====================Funkce pro zpo?d?n� 1 setiny==========================
cekej_setinu:
    push r24
    push r25
    push pomocny
    
    ; Vn?j?� smy?ka (?ek�n� cca 1/100 sekundy p?i 16 MHz)
    ldi r24, 207
vnorena_smycka1:
    ldi r25, 14
vnorena_smycka2:
    ldi pomocny, 14 ; 207x musi se odecist 14 ktery se kazdy musi odecist 14 (207*14*14 = 40572)
vnorena_smycka3:
    dec pomocny
    brne vnorena_smycka3
    
    dec r25
    brne vnorena_smycka2
    
    dec r24
    brne vnorena_smycka1
    
    pop pomocny
    pop r25
    pop r24
    ret
    
;====================Funkce pro zv�?en� ?asu o 1 setinu=======================
zvys_cas:
    ; Zv�?en� setin o 1
    inc setiny
    
    ; Kontrola p?ete?en� setin
    cpi setiny, 100 ; kdyz sto tak preskoci konec_zvyseni (nepreteklo a muzu jit vesele dal)
    brne konec_zvyseni
    
    ; Vynulov�n� setin a zv�?en� sekund
    clr setiny
    inc sekundy
    
    ; Kontrola p?ete?en� sekund
    cpi sekundy, 60
    brne konec_zvyseni
    
    ; Vynulov�n� sekund a zv�?en� minut
    clr sekundy
    inc minuty
    
    ; Kontrola p?ete?en� minut
    cpi minuty, 100
    brne konec_zvyseni
    
    ; Vynulov�n� minut p?i p?ete?en�
    clr minuty
    
konec_zvyseni:
    ret
    
;==================Funkce pro zobrazen� ?asu na displeji ==================
zobraz_cas:
    push pomocny
    push pozice
    
    ; Zobraz minuty (2 ?�slice)
    mov pomocny, minuty
    ldi pozice, 0
    call zobraz_2_cislice
    
    ; Zobraz dvojte?ku
    ldi pomocny, ':'
    ldi pozice, 2
    call show_char
    
    ; Zobraz sekundy (2 ?�slice)
    mov pomocny, sekundy
    ldi pozice, 3
    call zobraz_2_cislice
    
    ; Zobraz dvojte?ku
    ldi pomocny, '.'
    ldi pozice, 5
    call show_char
    
    ; Zobraz setiny (2 ?�slice)
    mov pomocny, setiny
    ldi pozice, 6
    call zobraz_2_cislice
    
    pop pozice
    pop pomocny
    ret
    
;-------------------Pomocn� funkce pro zobrazen� 2-cifern�ho ?�sla--------------
zobraz_2_cislice:
    push pomocny
    push pozice
    push r22
    
    ; V�po?et des�tek
    mov r22, pomocny
    ldi pomocny, 0
    
deleni_10:
    cpi r22, 10
    brlo konec_deleni
    subi r22, 10
    inc pomocny
    rjmp deleni_10
    
konec_deleni:
    ; Zobrazen� des�tek
    subi pomocny, -'0'  ; P?evod na ASCII
    call show_char
    
    ; Zobrazen� jednotek
    mov pomocny, r22
    subi pomocny, -'0'  ; P?evod na ASCII
    inc pozice
    call show_char
    
    pop r22
    pop pozice
    pop pomocny
    ret