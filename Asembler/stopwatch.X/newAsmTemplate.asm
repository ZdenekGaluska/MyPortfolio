.org 0x1000           ; Adresa za?átku programu
.include "printlib.inc"  ; Vlo?ení knihovny pro práci s displejem

.def setiny = r18        ; Registr pro setiny sekundy (0-99)
.def sekundy = r19       ; Registr pro sekundy (0-59)
.def minuty = r20        ; Registr pro minuty (0-99)
.def bezi = r21          ; P?íznak zda stopky b??í (0=stojí, 1=b??í)
.def pomocny = r16       ; Pomocný registr pro r?zné operace
.def pozice = r17        ; Pozice na displeji
.def tlacitko = r22      ; Pro ulo?ení hodnoty stisknutého tla?ítka
.def old_tlacitko = r23  ; Pro detekci zm?ny stavu tla?ítka    
.def mezicas = r24       ; 0 = normální zobrazení, 1 = zobrazení mezi?asu
    
; Konstanty pro kódy tla?ítek
.equ BTN_NONE = 0x0F     ; ?ádné tla?ítko
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
    clr mezicas           ; Inicializace p?íznaku mezi?asu
    ldi bezi, 1
    
;==========================HLAVNI SMYCKA===================================
;\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
hlavni_smycka:
    ; 1) Funkce pouziti tlacitek
    call zkontroluj_tlacitka
    
    ; 2) Kontrola jestli bezi cas, jinak se vrati na zacatek hlavni_smycky 
    cpi bezi, 1
    brne hlavni_smycka  ; Pokud neb??í, sko? zp?t na za?átek smy?ky
    
    ; 3) Pocita tak aby to spozdilo program o jednu setinu
    call cekej_setinu
    
    ; 4) zvysi cas o jednu setinu k casu
    call zvys_cas
    
    ; 5) zobrazi cas na displej
    cpi mezicas, 1        ; Je aktvivni mezicas?
    breq skip_display     ; pokud bylo tak to preskoci na skip_display
    call zobraz_cas       ; zobrazi cely cas
    
    skip_display:
    ; 6) Skok zp?t na za?átek hlavni smy?ky
    rjmp hlavni_smycka
    
;///////////////////////////////////////////////////////////////////////////
;========================KONEC HLAVNI SMYCKY===============================


;=====================Inicializace AD p?evodníku pro tla?ítka======================
; Tato funkce se volá na za?átku programu a nastavuje AD p?evodník
init_button:
    push pomocny
    
    ldi pomocny, (1<<ADEN) | (0b110<<ADPS0)
    sts ADCSRA, pomocny
    
    ldi pomocny, (0b01<<REFS0) | (1<<ADLAR)
    sts ADMUX, pomocny
    
    pop pomocny
    ret

;====================Funkce pro kontrolu tla?ítek==========================
zkontroluj_tlacitka:
    push pomocny
    ; ?ÁST 1: Zahájení p?evodu z analogového na digitální hodnotu
    lds pomocny, ADCSRA
    ori pomocny, (1<<ADSC)
    sts ADCSRA, pomocny
    ; ?ÁST 2: ?ekání na dokon?ení p?evodu
wait_conversion:
    lds pomocny, ADCSRA
    sbrc pomocny, ADSC
    rjmp wait_conversion
    ; ?ÁST 3: ?tení výsledku a identifikace tla?ítka
    lds pomocny, ADCH
    ; Posun o 4 bity doprava
    lsr pomocny
    lsr pomocny
    lsr pomocny
    lsr pomocny
    ; Ulo?ení kódu tla?ítka
    mov tlacitko, pomocny
    ; ?ÁST 4: Detekce zm?ny stavu tla?ítka
    cp tlacitko, old_tlacitko
    breq no_button_change
    ; Tla?ítko se zm?nilo, ulo?íme nový stav
    mov old_tlacitko, tlacitko
    
    ;----------------Logika tla?ítek - STOP (DOWN)---------------------
    cpi tlacitko, BTN_DOWN
    brne not_stop
    ; Tla?ítko DOWN bylo stisknuto - zastavíme stopky
    ldi bezi, 0
    rjmp button_done
not_stop:
    ; ----------------Logika tla?ítek - RESET (UP)----------------------
    cpi tlacitko, BTN_UP
    brne not_reset
    ; Tla?ítko UP bylo stisknuto - resetujeme stopky
    ldi bezi, 0
    clr setiny
    clr sekundy
    clr minuty
    clr mezicas         ; Zru?íme i p?ípadný mezi?as
    call zobraz_cas     ; Aktualizujeme displej
    rjmp button_done
not_reset:
    ;--------------Logika tla?ítek - START (LEFT)------------------------
    cpi tlacitko, BTN_LEFT
    brne not_start
    ; Tla?ítko LEFT bylo stisknuto - spustíme stopky
    ldi bezi, 1
    rjmp button_done
not_start:
    ;----------------Logika tla?ítek - MEZI?AS (SELECT)-----------------------
    cpi tlacitko, BTN_SELECT
    brne button_done
    ; Tla?ítko SELECT bylo stisknuto - p?epneme zobrazení mezi?asu
    ldi pomocny, 1
    eor mezicas, pomocny        ; P?epneme bit mezi?asu (0->1 nebo 1->0)
button_done:
no_button_change:
    pop pomocny
    ret
;====================Funkce pro zpo?d?ní 1 setiny==========================
cekej_setinu:
    push r24
    push r25
    push pomocny
    
    ; Vn?j?í smy?ka (?ekání cca 1/100 sekundy p?i 16 MHz)
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
    
;====================Funkce pro zvý?ení ?asu o 1 setinu=======================
zvys_cas:
    ; Zvý?ení setin o 1
    inc setiny
    
    ; Kontrola p?ete?ení setin
    cpi setiny, 100 ; kdyz sto tak preskoci konec_zvyseni (nepreteklo a muzu jit vesele dal)
    brne konec_zvyseni
    
    ; Vynulování setin a zvý?ení sekund
    clr setiny
    inc sekundy
    
    ; Kontrola p?ete?ení sekund
    cpi sekundy, 60
    brne konec_zvyseni
    
    ; Vynulování sekund a zvý?ení minut
    clr sekundy
    inc minuty
    
    ; Kontrola p?ete?ení minut
    cpi minuty, 100
    brne konec_zvyseni
    
    ; Vynulování minut p?i p?ete?ení
    clr minuty
    
konec_zvyseni:
    ret
    
;==================Funkce pro zobrazení ?asu na displeji ==================
zobraz_cas:
    push pomocny
    push pozice
    
    ; Zobraz minuty (2 ?íslice)
    mov pomocny, minuty
    ldi pozice, 0
    call zobraz_2_cislice
    
    ; Zobraz dvojte?ku
    ldi pomocny, ':'
    ldi pozice, 2
    call show_char
    
    ; Zobraz sekundy (2 ?íslice)
    mov pomocny, sekundy
    ldi pozice, 3
    call zobraz_2_cislice
    
    ; Zobraz dvojte?ku
    ldi pomocny, '.'
    ldi pozice, 5
    call show_char
    
    ; Zobraz setiny (2 ?íslice)
    mov pomocny, setiny
    ldi pozice, 6
    call zobraz_2_cislice
    
    pop pozice
    pop pomocny
    ret
    
;-------------------Pomocná funkce pro zobrazení 2-ciferného ?ísla--------------
zobraz_2_cislice:
    push pomocny
    push pozice
    push r22
    
    ; Výpo?et desítek
    mov r22, pomocny
    ldi pomocny, 0
    
deleni_10:
    cpi r22, 10
    brlo konec_deleni
    subi r22, 10
    inc pomocny
    rjmp deleni_10
    
konec_deleni:
    ; Zobrazení desítek
    subi pomocny, -'0'  ; P?evod na ASCII
    call show_char
    
    ; Zobrazení jednotek
    mov pomocny, r22
    subi pomocny, -'0'  ; P?evod na ASCII
    inc pozice
    call show_char
    
    pop r22
    pop pozice
    pop pomocny
    ret