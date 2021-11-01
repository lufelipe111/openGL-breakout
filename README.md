ABCg
======

Development framework accompanying the course MCTA008-17 Computer Graphics at [UFABC](https://www.ufabc.edu.br/).

----

### Aluno
- Nome: Luiz Felipe Leal Gomes Azzolini
- RA: 11048715 

### Sobre o projeto

O projeto feito para a disciplina de computação gráfica visava ser um clone do jogo Breakout da plataforma ATARI, explorando a biblioteca abcg feita para facilitar o uso e estudo de openGL.

O projeto é dividido em basicamente 5 elementos, são eles:
- Ball: responsavel por gerenciar e renderizar a bola e seus recursos, além de controlar os movimentos da bola.
- Bricks: responsável por gerenciar e renderizar os tijolos e seus recursos
- GameData: cria variáveis responsaveis por inputs do usuario
- OpenGLWindow: responsavel por chamar os métodos de todas as outras classes e tratar colisões
- Player: responsavel por gerenciar e renderizar o paddle do player, assim como monitorar inputs do usuario e controlar os movimentos do paddle

Alguns bugs foram nao foram solucionados a tempo, a maioria se concentrando em tratamento de colisões com os tijolos.

Outro bug notado foi ao compilar para webassembly, nesse caso os tijolos nao são renderizados corretamente, por outro lado ao compilar localmente os tijolos renderizam como deveriam.

### Funcionalidades faltantes
- [ ] Correção de bug de colisão com os tijolos
- [ ] Inicialização correta da bola no cenario
- [ ] Mecanica de quebrar tijolos
- [ ] Tela de Win/Game Over

### Link para projeto online
- [Github Pages](https://lufelipe111.github.io/openGL-breakout/)

### Credits

Developed by Harlen Batagelo.

### License

ABCg is licensed under the MIT License. See [LICENSE](https://github.com/hbatagelo/abcg/blob/main/LICENSE) for more information.
